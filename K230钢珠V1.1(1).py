"""
K230 钢珠检测 + 串口发送 + MJPEG 图传
=============================================
功能:
  1. YOLOv8 检测钢珠
  2. 计算钢珠中心 vs 凹槽中心(320,240) 的像素误差
  3. 通过 UART 发送误差值给 MSPM0G3507
  4. LCD 实时显示检测画面
  5. WiFi MJPEG 图传 (浏览器访问 http://<ip>:8080/)

串口协议 (ASCII, 115200 baud):
  检测到钢珠:  "error_x,error_y,conf\n"     例: "12,-5,0.93\n"
  未检测到:    "0,0,0.00\n"                 表示丢目标

   error_x > 0 → 钢珠偏右, 需要右倾
   error_x < 0 → 钢珠偏左, 需要左倾
   error_y > 0 → 钢珠偏下
   error_y < 0 → 钢珠偏上
"""

import ulab.numpy as np
import os, sys, gc
import network
import _thread
import socket
import time
import utime
from libs.PipeLine import PipeLine
from libs.YOLO import YOLOv8
from ybUtils.YbUart import YbUart
from media.display import Display
from media.media import *
from media.mjpeg import MJPEGEncoder


# ============================================================
# 视觉配置
# ============================================================
KMODEL_PATH      = "/sdcard/model.kmodel"
LABELS           = ["ball"]
MODEL_INPUT      = [640, 640]
RGB888P_SIZE     = [640, 480]
DISPLAY_SIZE     = [640, 480]
DISPLAY_MODE     = "lcd"           # "lcd" 支持 Display.writeback 图传
CONF_THRESH      = 0.5
NMS_THRESH       = 0.5
MAX_BOXES        = 1

# ============================================================
# 控制参数
# ============================================================
# 凹槽中心像素坐标 (画面正中心)
GROOVE_CENTER_X  = 320
GROOVE_CENTER_Y  = 240

# 死区 (像素): |error| < DEAD_ZONE 时强制输出0, 避免电机频繁抖动
DEAD_ZONE        = 3

# ============================================================
# 串口配置
# ============================================================
UART_BAUDRATE    = 115200
UART_ENABLE      = True          # True=发送, False=仅打印

# ============================================================
# 推理/显示参数
# ============================================================
INFER_INTERVAL   = 4             # 每 N 帧推理一次
GC_INTERVAL      = 30
FPS_LOG_INTERVAL = 30
DRAW_RESULT      = True

# ============================================================
# WiFi 配置
# ============================================================
WIFI_SSID        = "xiang"
WIFI_PASSWORD    = "aaaaaaaa"
WIFI_ENABLE      = True          # False=跳过WiFi, 仅本地运行

# ============================================================
# HTTP / MJPEG 配置
# ============================================================
SERVER_PORT        = 8080
STREAM_FPS         = 10
JPEG_QUALITY       = 40
REQUEST_TIMEOUT_MS = 1000
SEND_STALL_TIMEOUT_MS = 300
SEND_CHUNK_BYTES   = 4096
MAX_REQUEST_BYTES  = 4096

# 全局停止标志 (ai_worker 线程用)
ai_stop = False


# ============================================================
# HTML 页面 (CCW 90° 旋转 + 录像/下载)
# ============================================================
INDEX_HTML = b"""<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>K230 Steel Ball Camera</title>
<style>
*{box-sizing:border-box}
html,body{width:100%;height:100%;margin:0;background:#080808;color:#f5f5f5;
font-family:Arial,sans-serif;overflow:hidden}
body{display:grid;grid-template-rows:48px 1fr 50px}
header{display:flex;align-items:center;justify-content:space-between;padding:0 14px;
background:#151515;border-bottom:1px solid #2b2b2b;font-size:14px}
#status{color:#55dd77}
main{min-width:0;min-height:0;display:flex;align-items:center;justify-content:center;
position:relative;overflow:hidden}
#viewer{width:min(90vw,70vh);height:min(90vh,120vw);aspect-ratio:3/4;
position:relative;display:flex;align-items:center;justify-content:center;background:#000}
#stream{display:block;width:100%;height:100%;object-fit:contain;transform:rotate(-90deg)}
#ip{position:absolute;left:8px;top:8px;color:#55ff77;background:rgba(0,0,0,.65);
padding:3px 6px;border-radius:3px;font-size:13px;z-index:3}
#hint{position:absolute;bottom:8px;left:0;right:0;text-align:center;color:#999;
font-size:11px;z-index:3}
footer{display:flex;align-items:center;justify-content:center;gap:10px;
background:#151515;border-top:1px solid #2b2b2b}
button{border:1px solid #4772aa;border-radius:4px;background:#19314f;color:#fff;
padding:6px 12px;font-size:13px}
button:disabled{opacity:.5}
#recordStatus{font-size:12px;color:#ffcc66;min-width:90px}
</style>
</head>
<body>
<header><span>K230 Steel Ball AI Camera</span><span id="status">connecting</span></header>
<main>
  <div id="viewer">
    <img id="stream" alt="steel ball stream">
    <div id="hint">The web view is rotated CCW 90 degrees</div>
  </div>
</main>
<footer>
  <button id="recordButton">Start recording</button>
  <button id="downloadButton" disabled>Download video</button>
  <span id="recordStatus">not recording</span>
</footer>
<canvas id="recordCanvas" style="display:none"></canvas>
<script>
const stream=document.getElementById("stream");
const status=document.getElementById("status");
const recordButton=document.getElementById("recordButton");
const downloadButton=document.getElementById("downloadButton");
const recordStatus=document.getElementById("recordStatus");
const canvas=document.getElementById("recordCanvas");
const ctx=canvas.getContext("2d");
let streamUrl=null;
let activeController=null;
let connectionStartedAt=0;
let lastFrameAt=0;
let recorder=null;
let recordedChunks=[];
let recordedBlob=null;
let drawTimer=null;
let recordStarted=0;

function sleep(ms){return new Promise(resolve=>setTimeout(resolve,ms))}
function markerIndex(data,a,b,start){
  for(let i=start;i+1<data.length;i++){
    if(data[i]===a&&data[i+1]===b)return i;
  }
  return -1;
}
function appendBytes(left,right){
  const merged=new Uint8Array(left.length+right.length);
  merged.set(left); merged.set(right,left.length); return merged;
}
function showJpeg(bytes){
  const url=URL.createObjectURL(new Blob([bytes],{type:"image/jpeg"}));
  const old=streamUrl; streamUrl=url; stream.src=url;
  if(old)URL.revokeObjectURL(old);
  lastFrameAt=Date.now();
  status.textContent="online";
}
async function connectStream(){
  while(true){
    const controller=new AbortController();
    activeController=controller;
    connectionStartedAt=Date.now();
    lastFrameAt=0;
    try{
      status.textContent="connecting";
      const response=await fetch("/stream?_="+Date.now(),{cache:"no-store",signal:controller.signal});
      if(!response.ok||!response.body)throw new Error("stream unavailable");
      const reader=response.body.getReader();
      let buffer=new Uint8Array(0);
      while(true){
        const packet=await reader.read();
        if(packet.done)throw new Error("stream closed");
        buffer=appendBytes(buffer,packet.value);
        while(true){
          const start=markerIndex(buffer,0xff,0xd8,0);
          if(start<0){
            buffer=buffer.slice(Math.max(0,buffer.length-1)); break;
          }
          const end=markerIndex(buffer,0xff,0xd9,start+2);
          if(end<0){buffer=buffer.slice(start); break}
          showJpeg(buffer.slice(start,end+2));
          buffer=buffer.slice(end+2);
        }
      }
    }catch(error){
      status.textContent="reconnecting";
      if(activeController===controller)activeController=null;
      if(streamUrl){URL.revokeObjectURL(streamUrl);streamUrl=null}
      stream.removeAttribute("src");
      await sleep(500);
    }
  }
}

// Some browsers keep a failed fetch reader pending after a TCP half-close.
// Abort it explicitly when no JPEG has arrived for two seconds.
setInterval(()=>{
  if(!activeController)return;
  const reference=lastFrameAt||connectionStartedAt;
  if(Date.now()-reference>2000)activeController.abort();
},500);

function drawRotatedFrame(){
  if(!stream.complete || !stream.naturalWidth) return;
  const w=stream.naturalWidth;
  const h=stream.naturalHeight;
  if(canvas.width!==h || canvas.height!==w){canvas.width=h;canvas.height=w}
  ctx.fillStyle="#000";
  ctx.fillRect(0,0,canvas.width,canvas.height);
  ctx.save();
  ctx.translate(0,canvas.height);
  ctx.rotate(-Math.PI/2);
  ctx.drawImage(stream,0,0,w,h);
  ctx.restore();
}

function supportedMime(){
  // Prefer MP4/H.264 for Windows playback. Fall back only when the browser
  // does not expose an MP4 MediaRecorder encoder.
  const types=[
    "video/mp4;codecs=avc1.42E01E",
    "video/mp4",
    "video/webm;codecs=vp9",
    "video/webm;codecs=vp8",
    "video/webm"
  ];
  for(const type of types){
    if(MediaRecorder.isTypeSupported(type)) return type;
  }
  return "";
}

function startRecording(){
  if(!stream.complete || !stream.naturalWidth){
    recordStatus.textContent="waiting for video";
    return;
  }
  drawRotatedFrame();
  const fps=10;
  const mediaStream=canvas.captureStream(fps);
  const mime=supportedMime();
  const isMp4=mime.indexOf("video/mp4")===0;
  try{recorder=mime?new MediaRecorder(mediaStream,{mimeType:mime}):new MediaRecorder(mediaStream)}
  catch(error){recordStatus.textContent="recording unsupported";return}
  recordedChunks=[];
  recordedBlob=null;
  downloadButton.disabled=true;
  recorder.ondataavailable=(event)=>{if(event.data&&event.data.size)recordedChunks.push(event.data)};
  recorder.onstop=()=>{
    recordedBlob=new Blob(recordedChunks,{type:recorder.mimeType||"video/webm"});
    downloadButton.disabled=false;
    recordStatus.textContent=(recordedBlob.type.indexOf("mp4")>=0?"MP4":"WebM")+" ready";
    if(drawTimer){clearInterval(drawTimer);drawTimer=null}
    mediaStream.getTracks().forEach(track=>track.stop());
  };
  // Avoid timesliced MP4 fragments. A single finalized MP4 is more likely to
  // contain a complete duration/index and therefore supports seeking on
  // Windows players. WebM can still use short chunks safely.
  if(isMp4) recorder.start();
  else recorder.start(200);
  drawTimer=setInterval(drawRotatedFrame,1000/fps);
  recordStarted=Date.now();
  recordButton.textContent="Stop recording";
  recordStatus.textContent="recording 00:00";
}

function stopRecording(){
  if(!recorder||recorder.state!=="recording")return;
  recorder.stop();
  recordButton.textContent="Start recording";
}

recordButton.onclick=()=>{
  if(recorder&&recorder.state==="recording")stopRecording();
  else startRecording();
};
downloadButton.onclick=()=>{
  if(!recordedBlob)return;
  const url=URL.createObjectURL(recordedBlob);
  const link=document.createElement("a");
  link.href=url;
  const extension=recordedBlob.type.indexOf("mp4")>=0?"mp4":"webm";
  link.download="k230_steel_ball_"+Date.now()+"."+extension;
  link.click();
  setTimeout(()=>URL.revokeObjectURL(url),1000);
};
setInterval(()=>{
  if(recorder&&recorder.state==="recording"){
    const sec=Math.floor((Date.now()-recordStarted)/1000);
    recordStatus.textContent="recording "+String(Math.floor(sec/60)).padStart(2,"0")+":"+String(sec%60).padStart(2,"0");
  }
},500);
window.addEventListener("beforeunload",()=>{
  if(recorder&&recorder.state==="recording")recorder.stop();
});
connectStream();
</script>
</body>
</html>
"""


# ============================================================
# 辅助函数 (V1.1)
# ============================================================

def apply_dead_zone(value, dead_zone):
    """死区处理: |value| < dead_zone 时归零"""
    if value > dead_zone:
        return value
    elif value < -dead_zone:
        return value
    else:
        return 0


def format_uart_data(error_x, error_y, detected, conf):
    """
    格式化串口数据
    格式: "error_x,error_y,conf\n"
    未检测到: "0,0,0.00\n"
    """
    if detected:
        return "{},{},{:.2f}\n".format(error_x, error_y, conf)
    else:
        return "0,0,0.00\n"


# ============================================================
# 辅助函数 (图传)
# ============================================================

def safe_print_exc(error):
    """Print exception info – compatible with CanMV which lacks sys.print_exception."""
    try:
        sys.print_exception(error)
    except AttributeError:
        try:
            import traceback
            traceback.print_exc()
        except ImportError:
            print("ERROR:", type(error).__name__, error)


def connect_network():
    """WiFi STA 连接 (RT-Smart 模式)"""
    netif = network.WLAN(0)
    print("[WIFI] connecting:", WIFI_SSID)
    if netif.isconnected():
        print("[WIFI] disconnecting previous session")
        netif.disconnect()
        time.sleep(1)
    netif.connect(WIFI_SSID, WIFI_PASSWORD)
    while netif.ifconfig()[0] == "0.0.0.0":
        os.exitpoint()
        time.sleep_ms(200)
    ip = netif.ifconfig()[0]
    print("[WIFI] connected, ifconfig:", netif.ifconfig())
    return netif, ip


def draw_ip(osd_img, ip):
    """在 OSD 左上角显示 IP 地址"""
    osd_img.draw_string_advanced(
        8, 8, 24,
        "IP: " + ip,
        color=(0, 255, 80, 255),
    )


def capture_jpeg(pipeline, encoder, osd_lock):
    """捕获当前显示帧并编码为 JPEG"""
    started = time.ticks_ms()
    osd_lock.acquire()
    try:
        pipeline.show_image()
        display_frame = Display.writeback_dump(1000)
    finally:
        osd_lock.release()

    if display_frame is None:
        raise RuntimeError("display writeback timeout")
    jpeg = encoder.encode(display_frame, timeout_ms=1000)
    del display_frame
    return jpeg, time.ticks_diff(time.ticks_ms(), started)


def send_all(client, data):
    """分块发送数据, 容忍 socket 缓冲区满"""
    view = memoryview(data)
    offset = 0
    deadline = time.ticks_add(time.ticks_ms(), SEND_STALL_TIMEOUT_MS)
    while offset < len(view):
        try:
            end = min(offset + SEND_CHUNK_BYTES, len(view))
            sent = client.send(view[offset:end])
            if sent:
                offset += sent
                deadline = time.ticks_add(time.ticks_ms(), SEND_STALL_TIMEOUT_MS)
                continue
        except OSError as error:
            if error.errno not in (11, 110):
                raise
        if time.ticks_diff(deadline, time.ticks_ms()) <= 0:
            raise OSError(110)
        os.exitpoint()
        time.sleep_ms(1)


def read_path(client):
    """非阻塞读取 HTTP 请求行, 返回路径"""
    request = bytearray()
    deadline = time.ticks_add(time.ticks_ms(), REQUEST_TIMEOUT_MS)
    client.setblocking(False)
    while len(request) < MAX_REQUEST_BYTES:
        try:
            chunk = client.recv(min(256, MAX_REQUEST_BYTES - len(request)))
        except OSError as error:
            if error.errno == 110 and len(request) == 0:
                raise
            if error.errno != 11:
                raise
            chunk = None
        if chunk:
            request.extend(chunk)
            if request.find(b"\r\n\r\n") >= 0:
                break
        elif time.ticks_diff(deadline, time.ticks_ms()) <= 0:
            raise OSError(110)
        else:
            os.exitpoint()
            time.sleep_ms(10)
    request_line = bytes(request).split(b"\r\n", 1)[0].split()
    if len(request_line) < 2:
        return "/"
    return request_line[1].decode().split("?", 1)[0]


def send_page(client, ip):
    """发送 HTML 索引页面"""
    page = INDEX_HTML
    send_all(
        client,
        b"HTTP/1.1 200 OK\r\n"
        b"Content-Type: text/html; charset=utf-8\r\n"
        b"Cache-Control: no-store\r\n"
        b"Connection: close\r\n\r\n" + page,
    )


def stream_mjpeg(client, pipeline, encoder, osd_lock):
    """MJPEG 视频流"""
    send_all(
        client,
        b"HTTP/1.1 200 OK\r\n"
        b"Content-Type: multipart/x-mixed-replace; boundary=frame\r\n"
        b"Cache-Control: no-store, no-cache, must-revalidate\r\n"
        b"Pragma: no-cache\r\nConnection: close\r\n\r\n",
    )
    interval = 1000 // STREAM_FPS
    count = 0
    stat_started = time.ticks_ms()
    capture_total = 0
    send_total = 0
    print("[HTTP] steel-ball MJPEG client started")
    while True:
        loop_started = time.ticks_ms()
        jpeg, capture_ms = capture_jpeg(pipeline, encoder, osd_lock)
        jpeg_size = len(jpeg)
        part_header = (
            "--frame\r\nContent-Type: image/jpeg\r\n"
            "Content-Length: %d\r\n\r\n" % jpeg_size
        )
        send_started = time.ticks_ms()
        try:
            send_all(client, part_header.encode())
            send_all(client, jpeg)
            send_all(client, b"\r\n")
        except OSError as error:
            print("[HTTP] client dropped, send errno:", error.errno)
            del jpeg
            raise
        send_ms = time.ticks_diff(time.ticks_ms(), send_started)
        del jpeg
        count += 1
        capture_total += capture_ms
        send_total += send_ms
        if count % 10 == 0:
            elapsed = max(1, time.ticks_diff(time.ticks_ms(), stat_started))
            print(
                "[HTTP] frames=%d fps=%.1f jpeg=%dB capture=%dms send=%dms"
                % (count, count * 1000.0 / elapsed, jpeg_size,
                   capture_total // count, send_total // count)
            )
            gc.collect()
        delay = interval - time.ticks_diff(time.ticks_ms(), loop_started)
        if delay > 0:
            time.sleep_ms(delay)
        os.exitpoint()


def client_worker(client, pipeline, encoder, osd_lock, ip, stream_lock):
    """处理单个浏览器连接 (运行在独立线程)"""
    try:
        path = read_path(client)
        print("[HTTP] request:", path)
        if path == "/stream":
            stream_lock.acquire()
            try:
                stream_mjpeg(client, pipeline, encoder, osd_lock)
            finally:
                stream_lock.release()
        else:
            send_page(client, ip)
    except OSError as error:
        print("[HTTP] client ended, errno:", error.errno)
    except BaseException as error:
        print("[HTTP] client error:", error)
        safe_print_exc(error)
    finally:
        client.close()


# ============================================================
# AI 工作线程 (检测 + 误差计算 + 串口 + OSD 绘制)
# ============================================================

def ai_worker(pipeline, detector, ip, osd_lock, uart):
    """
    AI 检测线程 (从 V1.1 合并):
      - 每 INFER_INTERVAL 帧推理一次
      - 计算钢珠中心误差 → 死区 → UART 发送
      - 每帧清屏 + 画检测框 + 十字线 + IP + show_image
    """
    global ai_stop
    frame_count = 0
    result = None

    # V1.1 丢目标保持逻辑
    last_error_x = 0
    last_error_y = 0
    last_conf = 0.0
    loss_timeout = 10
    loss_count = 0

    fps_start_ms = utime.ticks_ms()

    while not ai_stop:
        frame = None
        try:
            frame = pipeline.get_frame()
            if frame is None:
                time.sleep_ms(5)
                continue

            frame_count += 1

            # ---- 推理 (每 INFER_INTERVAL 帧) ----
            if result is None or frame_count % INFER_INTERVAL == 0:
                result = detector.run(frame)

                if len(result[0]) > 0:
                    # 检测到钢珠
                    loss_count = 0
                    bbox = result[0][0]       # [x1, y1, w, h]
                    conf = result[2][0]        # 置信度
                    bbox_list = bbox.tolist()

                    # 钢珠中心坐标
                    cx = bbox_list[0] + bbox_list[2] / 2
                    cy = bbox_list[1] + bbox_list[3] / 2

                    # 计算误差 (钢珠位置 - 凹槽中心)
                    raw_ex = cx - GROOVE_CENTER_X
                    raw_ey = cy - GROOVE_CENTER_Y

                    # 死区处理
                    error_x = apply_dead_zone(raw_ex, DEAD_ZONE)
                    error_y = apply_dead_zone(raw_ey, DEAD_ZONE)

                    # 保存有效值
                    last_error_x = error_x
                    last_error_y = error_y
                    last_conf = conf

                    # 调试打印
                    print("x:{:.1f} y:{:.1f} err_x:{} err_y:{} conf:{:.2f}".format(
                        cx, cy, error_x, error_y, conf))

                else:
                    # 未检测到钢珠
                    loss_count += 1
                    if loss_count > loss_timeout:
                        last_error_x = 0
                        last_error_y = 0
                        last_conf = 0.0
                    print("loss:{} hold_err_x:{}".format(loss_count, last_error_x))

                # ---- 发送串口 ----
                if UART_ENABLE and uart is not None:
                    if loss_count > loss_timeout:
                        uart_data = format_uart_data(0, 0, False, 0.0)
                    else:
                        uart_data = format_uart_data(
                            last_error_x, last_error_y, True, last_conf)
                    uart.send(uart_data)

            # ---- OSD 绘制 (每帧, 用 osd_lock 保护) ----
            osd_lock.acquire()
            try:
                pipeline.osd_img.clear()

                # 画检测框
                if DRAW_RESULT and result is not None:
                    detector.draw_result(result, pipeline.osd_img)

                # 画凹槽中心十字线 (浅灰色半透明)
                cx_c = GROOVE_CENTER_X
                cy_c = GROOVE_CENTER_Y
                pipeline.osd_img.draw_line(
                    cx_c - 15, cy_c, cx_c + 15, cy_c,
                    color=(80, 200, 200, 200), thickness=1)
                pipeline.osd_img.draw_line(
                    cx_c, cy_c - 15, cx_c, cy_c + 15,
                    color=(80, 200, 200, 200), thickness=1)

                # 画 IP 地址 (WiFi 启用时)
                if ip:
                    draw_ip(pipeline.osd_img, ip)

                # 提交 OSD 到显示
                pipeline.show_image()
            finally:
                osd_lock.release()

            # ---- GC / FPS ----
            if frame_count % GC_INTERVAL == 0:
                gc.collect()
            if frame_count % FPS_LOG_INTERVAL == 0:
                now_ms = utime.ticks_ms()
                elapsed_ms = utime.ticks_diff(now_ms, fps_start_ms)
                if elapsed_ms > 0:
                    fps = FPS_LOG_INTERVAL * 1000 / elapsed_ms
                    print("display fps: {:.1f}".format(fps))
                fps_start_ms = now_ms

        except BaseException as error:
            print("[AI] worker error:", error)
            time.sleep_ms(50)
        finally:
            if frame is not None:
                del frame


# ============================================================
# 主函数
# ============================================================

def main():
    global ai_stop
    netif = None
    pipeline = None
    detector = None
    encoder = None
    server = None
    uart = None
    osd_lock = None
    stream_lock = None
    ai_thread_started = False
    writeback_enabled = False
    ip = None

    try:
        # ========================================================
        # 1. 初始化串口
        # ========================================================
        if UART_ENABLE:
            print("[UART] 初始化串口 {} baud...".format(UART_BAUDRATE))
            uart = YbUart(baudrate=UART_BAUDRATE)
            print("  ✓ 串口就绪")

        # ========================================================
        # 2. 连接 WiFi (可选)
        # ========================================================
        if WIFI_ENABLE:
            netif, ip = connect_network()

        # ========================================================
        # 3. 初始化摄像头
        # ========================================================
        pipeline = PipeLine(
            rgb888p_size=RGB888P_SIZE,
            display_size=DISPLAY_SIZE,
            display_mode=DISPLAY_MODE,
        )
        pipeline.create(to_ide=False)

        # ========================================================
        # 4. 初始化 YOLO
        # ========================================================
        detector = YOLOv8(
            task_type="detect", mode="video", kmodel_path=KMODEL_PATH,
            labels=LABELS, rgb888p_size=RGB888P_SIZE,
            model_input_size=MODEL_INPUT, display_size=DISPLAY_SIZE,
            conf_thresh=CONF_THRESH, nms_thresh=NMS_THRESH,
            max_boxes_num=MAX_BOXES, debug_mode=0
        )
        detector.config_preprocess()

        # ========================================================
        # 5. 初始化 MJPEG 编码器 + 写回 (图传用)
        # ========================================================
        if WIFI_ENABLE:
            encoder = MJPEGEncoder(quality=JPEG_QUALITY)
            if not Display.writeback(True):
                raise RuntimeError("start display writeback failed")
            writeback_enabled = True

        # ========================================================
        # 6. 预热帧
        # ========================================================
        for _ in range(5):
            pipeline.get_frame()

        # ========================================================
        # 7. 启动 AI 工作线程
        # ========================================================
        osd_lock = _thread.allocate_lock()
        stream_lock = _thread.allocate_lock() if WIFI_ENABLE else None
        ai_stop = False
        _thread.start_new_thread(
            ai_worker, (pipeline, detector, ip, osd_lock, uart))
        ai_thread_started = True

        print("=" * 50)
        print("  钢珠检测 + 误差输出 + MJPEG 图传")
        print("  凹槽中心: ({}, {})".format(GROOVE_CENTER_X, GROOVE_CENTER_Y))
        print("  死区: {}px".format(DEAD_ZONE))
        print("  UART: {}baud".format(UART_BAUDRATE))
        print("  WiFi: {}".format("启用" if WIFI_ENABLE else "禁用"))
        if WIFI_ENABLE:
            print("  http://{}:{}/".format(ip, SERVER_PORT))
        print("=" * 50)

        # ========================================================
        # 8. 验证 JPEG 捕获 + HTTP 服务器
        # ========================================================
        if WIFI_ENABLE:
            time.sleep_ms(100)
            first_jpeg, first_ms = capture_jpeg(pipeline, encoder, osd_lock)
            print("[INIT] first JPEG:", len(first_jpeg), "bytes,", first_ms, "ms")
            del first_jpeg
            gc.collect()

            server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            server.bind(socket.getaddrinfo("0.0.0.0", SERVER_PORT)[0][-1])
            server.listen(4)
            server.setblocking(False)
            print("[HTTP] open http://%s:%d/" % (ip, SERVER_PORT))
            print("[HTTP] web view rotation: CCW 90 degrees")

            # ---- HTTP accept 循环 ----
            while True:
                try:
                    accepted_client, address = server.accept()
                except OSError as error:
                    if error.errno not in (11, 103, 104, 110):
                        raise
                    os.exitpoint()
                    time.sleep_ms(20)
                    continue
                print("[HTTP] accepted:", address)
                _thread.start_new_thread(
                    client_worker,
                    (accepted_client, pipeline, encoder, osd_lock, ip, stream_lock),
                )
                gc.collect()
        else:
            # 无 WiFi: 主线程空闲等待
            while True:
                os.exitpoint()
                time.sleep_ms(500)

    except KeyboardInterrupt:
        print("\n用户退出")
    except Exception as error:
        sys.print_exception(error)
    finally:
        # ---- 清理 (顺序很重要) ----
        ai_stop = True
        if ai_thread_started:
            time.sleep_ms(100)
        if server is not None:
            server.close()
        if encoder is not None:
            encoder.close()
        if detector is not None:
            detector.deinit()
        if writeback_enabled:
            Display.writeback(False)
        if pipeline is not None:
            pipeline.destroy()
        if uart is not None:
            try:
                uart.deinit()
            except:
                pass
        os.exitpoint(os.EXITPOINT_ENABLE_SLEEP)
        time.sleep_ms(100)
        print("清理完成")


if __name__ == "__main__":
    os.exitpoint(os.EXITPOINT_ENABLE)
    main()
