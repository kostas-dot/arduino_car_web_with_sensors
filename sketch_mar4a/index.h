const char *HTML_CONTENT = R"=====(
<!DOCTYPE html>
<html>

<head>
    <title>Arduino Control Car via Web</title>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <style type="text/css">
        body {
            text-align: center;
            font-size: 24px;
        }

        button {
            text-align: center;
            font-size: 24px;
        }

        #container {
            display: grid;
            grid-template-columns: 1fr 1fr 1fr;
            margin-block-start: auto;
            margin-block-end: auto;
            margin-right: auto;
            margin-left: auto;
        }

        div[class^='button'] {
            margin: auto;
            margin-top: auto;
            margin-bottom: auto;
            margin-left: 100px;
            margin-right: auto;
            margin-block-start: 10px;
            margin-block-end: 10px;
            width: auto;
        }

        .button_STL {
            background-color: #058ec4;
            /* Green */
            border: none;
            color: white;
            padding: 20px 30px;
            text-align: center;
            text-decoration: none;
            display: inline-block;
            font-size: 16px;
            font-size: 24px;
            cursor: pointer;
            outline: auto;
            border-radius: 300px;
            box-shadow: 0 9px #095cc9;
            width: 280px;
            background-size: contain;
            transform: translate(-50%, -50%);
        }

        .button_STL:hover {
            background-color: #5239c4;
            /* Green */
            color: rgb(29, 201, 207);
        }

        .button_STL:active {
            background-color: #89bdd4;
            box-shadow: 0 5px #8aa8b4;
        }
    </style>
    <script>
        var CMD_STOP = 0;
        var CMD_FORWARD = 1;
        var CMD_BACKWARDS = 2;
        var CMD_LEFT = 4;
        var CMD_RIGHT = 8;
        var ws = null;

        function init() {

            var container = document.querySelector("#container");
            container.addEventListener("touchstart", mouse_down);
            container.addEventListener("touchend", mouse_up);
            container.addEventListener("touchcancel", mouse_up);
            container.addEventListener("mousedown", mouse_down);
            container.addEventListener("mouseup", mouse_up);
            container.addEventListener("mouseout", mouse_up);
        }
        function ws_onmessage(e_msg) {
            e_msg = e_msg || window.event; // MessageEvent

            <!-- alert("msg : " + e_msg.data); -->
        }
        function ws_onopen() {
            document.getElementById("ws_state").innerHTML = "OPEN";
            document.getElementById("wc_conn").innerHTML = "Disconnect";
        }
        function ws_onclose() {
            document.getElementById("ws_state").innerHTML = "CLOSED";
            document.getElementById("wc_conn").innerHTML = "Connect";
            console.log("socket was closed");
            ws.onopen = null;
            ws.onclose = null;
            ws.onmessage = null;
            ws = null;
        }
        function wc_onclick() {
            if (ws == null) {
                ws = new WebSocket("ws://" + window.location.host + ":81");
                document.getElementById("ws_state").innerHTML = "CONNECTING";

                ws.onopen = ws_onopen;
                ws.onclose = ws_onclose;
                ws.onmessage = ws_onmessage;
            }
            else
                ws.close();
        }
        function mouse_down(event) {
            if (event.target !== event.currentTarget) {
                var id = event.target.id;
                send_command(event,id);
            }
            event.stopPropagation();
            event.preventDefault();
        }

        function mouse_up(event) {
            if (event.target !== event.currentTarget) {
                var id = event.target.id;
                send_command(CMD_STOP);
            }
            event.stopPropagation();
            event.preventDefault();
        }
        function send_command(event,cmd) {
          while(event.isTrusted){
            if (ws != null)
                if (ws.readyState == 1){
                      ws.send(cmd + "\r\n");
                      }
                      if (event.target !== event.currentTarget)break;}
        }

        window.onload = init;
    </script>
</head>

<body style="max-width:100%; height:auto;">
    <h2>Arduino Car via Web</h2>
    <div id="container" style="max-width:100%; height:auto;position: relative;">
        <div></div>
        <div id="1" class="button_STL">FORWARD</div>
        <div></div>
        <div id="4" class="button_STL">TURNLEFT</div>
        <div id="0" class="button_STL">STOP</div>
        <div id="8" class="button_STL">TURNRIGHT</div>
        <div></div>
        <div id="2" class="button_STL">BACKWARDS</div>
        <div></div>
    </div>
    <p style="position: relative; ">
        WebSocket : <span id="ws_state" style="color:blue">CLOSED</span><br>
    </p>
    <button id="wc_conn" type="button" onclick="wc_onclick();">Connect</button>
    <br>
</body>

</html>
)=====";
