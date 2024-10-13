# -*- coding: utf-8 -*-
#   ↑ UTF8使用のためのおまじない

import os
# このファイルが存在するディレクトリ
DIRECTORY = os.path.dirname(os.path.abspath(__file__))

import clr
clr.AddReference("Renode-peripherals")
clr.AddReference("IronPython.StdLib")


import SimpleHTTPServer
import SocketServer
from threading import Thread
import base64
import tempfile

from Antmicro.Renode.Peripherals.Miscellaneous import LED, Button

# get current path, 0 is for Renode directory, 1 for cwd
cwd = monitor.CurrentPathPrefixes[1]
sys.path.append(cwd)

# カレントディレクトリをこのファイルが存在するディレクトリにする
# (サーバの参照するrootを変更するため)
os.chdir(DIRECTORY)

from websocket_server import WebsocketServer

# WebSocketメッセージ送信
def send_message(message):
    if websocket_server is not None and len(websocket_server.clients) > 0:
        for client in websocket_server.clients:
            try:
                websocket_server.send_message(client, message)
            except Exception as e:
                # in case something got disconnected
                pass

# WebSocketメッセージ受信時のハンドラ
def message_received(client, server, message):
    splitted = message.split("|")
    type = splitted[0]
    
    if type == "BUTTON":
        # ボタンからのメッセージ
        name = splitted[1]
        state = splitted[2]
        
        btn = buttons.get(name)
        if btn is not None :
            if state == 'ON' :
                # ボタンが押された
                btn.Press()
                print "Button " + name + " is pressed."
            else :
                # ボタンが離された
                btn.Release()
                print "Button " + name + " is released."
        else :
            # 存在しないボタン
            print "Button " + name + " is not defined. Ignored."
    else: 
        # 不明なメッセージ
        if len(message) > 200:
            message = message[:200]+'..'
        print "Client(%d) said: %s" % (client['id'], message)

# LEDの状態変化時のハンドラ
def blink_led(led, state):
    # print("|".join(["LED", machine.GetLocalName(led), str(state)]))
    send_message("|".join(["LED", machine.GetLocalName(led), str(state)]))

main_server = None
server_thread = None
websocket_server = None
websocket_thread = None

machine = None

buttons = {}

# mc_XXXX 関数はコマンドXXXXとしてコンソールから実行できる
print "Adding serveVisualization command."
def mc_serveVisualization(port):
    global main_server
    global server_thread
    global websocket_server
    global websocket_thread
    global machine
    global buttons

    # get emulation
    emulation = Antmicro.Renode.Core.EmulationManager.Instance.CurrentEmulation

    # Assume a single machine for now.
    machine = emulation.Machines[0]

    # すべてのLEDに状態変化時のハンドラを登録
    leds = machine.GetPeripheralsOfType[LED]()
    for led in leds:
        led.StateChanged += blink_led
        
    # Buttonの一覧を作成
    btns = machine.GetPeripheralsOfType[Button]()
    for btn in btns:
        """
        # debug : インスタンスの持つmethod等を表示して確認
        print type(btn)
        for x in dir(btn):
            print x
        """
        name = machine.GetLocalName(btn)
        # print name
        buttons[name] = btn
        
    # setting up server threads
    websocket_server = WebsocketServer(9001)
    websocket_server.set_fn_message_received(message_received)
    websocket_thread = Thread(target=websocket_server.serve_forever)
    websocket_thread.deamon = True
    websocket_thread.start()

    try:
        main_server = SocketServer.TCPServer(("", port), SimpleHTTPServer.SimpleHTTPRequestHandler)
        server_thread = Thread(target = main_server.serve_forever)
        server_thread.deamon = True

        print "Serving interactive visualization at port", port
        server_thread.start()
    except:
        import traceback
        traceback.print_exc()

print "Adding stopVisualization command."
def mc_stopVisualization():
    global main_server
    global server_thread
    global websocket_server
    global websocket_thread

    if main_server is None:
        print "Start visualization with `serveVisualization {port}` first"
        return
        
    main_server.shutdown()
    main_server.server_close()
    websocket_server.close_server()
    server_thread.join()
    websocket_thread.join()
    main_server = None
    server_thread = None
    websocket_server = None
    websocket_thread = None
