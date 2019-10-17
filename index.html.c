#include <pgmspace.h>
char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
      <head>
      <meta charset="utf-8"> <title>Wifi PPM (V1.1)</title>
      <meta name="viewport" content="width=device-width, user-scalable=no">
  
      <style>
  
      .switch, .voltage {
        position: relative;
        display: block;
        margin-left: auto;
        margin-right: auto;
        width: 34px;
        height: 34px;
      }

      .voltage {
        position: relative;
        display: block;
        margin-left: auto;
        margin-right: auto;
        width: 54px;
        height: 34px;
      }

      .switch input {display:none;}
  
      .slider {
        position: absolute;
        cursor: pointer;
        border-radius: 34px;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #F00;
      }
  
      input:checked + .slider {
        background-color: #0F0;
      }
  
      </style>
      </head>
      <body onload="start()">
       <div class="voltage" id="vbat_min_div" style="display: none">VMin: <input id="vbat_min" type="number" min="0.1" max="20.0" step="0.1" value="6.3" style="width: 50px"/>
       </div>
      <br>
      <label class="switch">
        <input type="checkbox" id="Button0" onchange="Button1Change(this)">
        <span class="slider"></span>
      </label>
      <br>
      <label class="switch">
        <input type="checkbox" id="Button1" onchange="Button2Change(this)">
        <span class="slider"></span>
      </label>
      <br>
      <label class="switch">
        <input type="checkbox" id="Button2" onchange="Button3Change(this)">
        <span class="slider"></span>
      </label>
      <br>
      <label class="switch">
        <input type="checkbox" id="Button3" onchange="Button4Change(this)">
        <span class="slider"></span>
      </label>
<!--       <br>
       <div class="voltage" id="vbat_text" style="display: none"> VBat: </div>
       <br> -->
        <div class="voltage" id="vbat" style="display: none">0 V</div>


  
      <div id="invert1div" style="display: none; position: absolute; left: 20%; top: 1%;">
        Invert Y Axis<input type="checkbox" id="invert1">
      </div>
      <div id="invert2div" style="display: none; position: absolute; left: 70%; top: 1%;">
        Invert Y Axis<input type="checkbox" id="invert2">
      </div>
  
      <div id="trimdiv" style="display:none;">
        <input type="range" min="-100" max="100" value="0" class="slider" id="trim1x" style="position: absolute; left: 5%; top: 95%; width: 38%;">
        <input type="range" min="-100" max="100" value="0" class="slider" id="trim2x" style="position: absolute; left: 55%; top: 95%; width: 38%;">
        <input type="range" min="-100" max="100" value="0" class="slider" id="trim2y" orient="vertical" style="position: absolute; left: 98%; top: 10%; height: 78%;-webkit-appearance: slider-vertical;">
        <input type="range" min="-100" max="100" value="0" class="slider" id="trim1y" orient="vertical" style="position: absolute; left: 1%; top: 10%; height: 78%;-webkit-appearance: slider-vertical;">
      </div>
      <div id=textdiv style="position:absolute;top:0px;left:0px;"></div>
      <canvas id="Canvas_left" style="border:1px solid #d3d3d3;position:absolute; top:10%; left:5%; z-index:0">
      Your browser does not support the HTML5 canvas tag.</canvas>
      <canvas id="Canvas_right" style="border:1px solid #d3d3d3;position:absolute; top:10%; left:55%; z-index:0">
      Your browser does not support the HTML5 canvas tag.</canvas>
      <canvas id="Canvas_stickl" style="position:absolute; top:10%; left:5%; z-index:1">
      Your browser does not support the HTML5 canvas tag.</canvas>
      <canvas id="Canvas_stickr" style="position:absolute; top:10%; left:55%; z-index:1">
      Your browser does not support the HTML5 canvas tag.</canvas>
  
      <script>
      var touches = [];
      var w = 0;
      var wsconnect = 0;
      var h = 0;
      var end=0;
      var ctx_left;
      var ctx_right;
      var ctx_stickl;
      var ctx_stickr;
      var gamepads = {};
      var buttons=[0,0,0,0];
      var ppm=[1100,1100,1100,1100,1100,1100,1100,1100];
      var oldppm=[0,0,0,0,0,0,0,0];
      var getInterval;
      var aliveInterval;
      var visible=0;
       var alarm;
      var alarmfreq=0;

  //var connection = new WebSocket('ws://' + window.location.host + ':81',"dumb-increment-protocol"); n"
      var connection = new WebSocket('ws://' + window.location.host + ':81', ['arduino']);
//      var connection = new WebSocket('ws://192.168.4.1:81', ['arduino']);
  
      connection.onopen = function () {         //open
        console.log("Websocket Open");
        wsconnect=1;
        window.requestAnimationFrame(update);
        draw_stick(ctx_stickl,ctx_stickl.canvas.width/2,ctx_stickl.canvas.height,0,1);
        draw_stick(ctx_stickr,ctx_stickr.canvas.width/2,ctx_stickr.canvas.height/2,2,3);
        aliveInterval=setInterval(sendalive,1000);
//        getInterval=setInterval(getVBat, 5000);
      };
  
      connection.onerror = function (error) {   //error
        console.log('WebSocket Error ' + error);
        wsconnect=0;
        draw_stick(ctx_stickl,ctx_stickl.canvas.width/2,ctx_stickl.canvas.height,0,1);
        draw_stick(ctx_stickr,ctx_stickr.canvas.width/2,ctx_stickr.canvas.height/2,2,3);
        clearInterval(getInterval);
          gainNode.disconnect(audioCtx.destination);
          if(alarmfreq!=0){
            clearInterval(alarm);
            alarmfreq=0;
        }
      };
  
      connection.onmessage = function (e) {     //Daten empfangen
         console.log("indata: " + e.data);
         if(e.data!="Connected"){
            var indata=JSON.parse(e.data);
              if(indata.vbat<=20){
                console.log("No bat");
                if(visible==1){
//                document.getElementById("vbat_text").style.display="none";
                  document.getElementById("vbat").style.display="none";
                  document.getElementById("vbat_min_div").style.display="none";
                  visible=0;
                      gainNode.disconnect(audioCtx.destination);
                      if(alarmfreq!=0){
                        clearInterval(alarm);
                        alarmfreq=0;
                    }
                  }
                }
              else{
                if(visible==0){
                  document.getElementById("vbat_min_div").style.display="block";
                  document.getElementById("vbat").style.display="block";
                  visible=1;
//                  document.getElementById("vbat_text").style.display="block";
                }
                var vbat=indata.vbat/10;
                document.getElementById("vbat").innerHTML = vbat + " V";
                if(vbat<document.getElementById("vbat_min").value){
                   if(alarmfreq==0){
                      gainNode.connect(audioCtx.destination);
                      alarm=setInterval(function(){
                      navigator.vibrate(500); // vibrate for 200ms
                         if(alarmfreq==1000)
                          alarmfreq=2000;
                        else
                          alarmfreq=1000;
                        oscillator.frequency.value=alarmfreq;
                      }, 1000);
                    }                    
                }
                else{
                    gainNode.disconnect(audioCtx.destination);
                    if(alarmfreq!=0){
                      clearInterval(alarm);
                      alarmfreq=0;
                    }
              }
          }
         }
      }
  
      connection.onclose = function (e)
      {
          console.log("Websocket close");
        wsconnect=0;
        draw_stick(ctx_stickl,ctx_stickl.canvas.width/2,ctx_stickl.canvas.height,0,1);
        draw_stick(ctx_stickr,ctx_stickr.canvas.width/2,ctx_stickr.canvas.height/2,2,3);
        clearInterval(getInterval);
      }
  
      function start()
      {
        try {
            // Fix up for prefixing
            window.AudioContext = window.AudioContext||window.webkitAudioContext;
            context = new AudioContext();
        }
        catch(e) {
            alert('Web Audio API is not supported in this browser -> no audio\n Try Google Chrome or Firefox instead.');
          }

           audioCtx=new(window.AudioContext || window.webkitAudioContext)();
          oscillator=audioCtx.createOscillator();
          gainNode=audioCtx.createGain();
          gainNode.gain.value = 0.5;
      
          oscillator.connect(gainNode);
          oscillator.type='sine';
          oscillator.frequency.value=1000;
          oscillator.start(0);
  
        
        var c_left = document.getElementById("Canvas_left");
        ctx_left = c_left.getContext("2d");
  
        var c_right = document.getElementById("Canvas_right");
        ctx_right = c_right.getContext("2d");
  
        var c_stickl = document.getElementById("Canvas_stickl");
        ctx_stickl = c_stickl.getContext("2d");
  
        var c_stickr = document.getElementById("Canvas_stickr");
        ctx_stickr = c_stickr.getContext("2d");
  
        update();
  
        draw_background(ctx_left);
        draw_background(ctx_right);
        draw_stick(ctx_stickl,c_stickl.width/2,c_stickl.height,0,1);
        draw_stick(ctx_stickr,c_stickr.width/2,c_stickr.height/2,2,3);
  
        window.addEventListener("optimizedResize", function() {
          resize();
        });  
        window.addEventListener("orientationchange", function() {
          window.setTimeout(resize, 300)
        });    
        c_stickl.addEventListener('touchend', function() {
          console.log( "endl");
        });
        c_stickl.addEventListener('touchmove', function(event) {
          event.preventDefault();
          touches = event.touches;
          window.requestAnimationFrame(update);
        });
        c_stickl.addEventListener('touchstart', function(event) {
          console.log('startl');
        });
        c_stickr.addEventListener('touchend', function() {
          console.log("endr");
          end=1;
          draw_stick(ctx_stickr,c_stickr.width/2,c_stickr.height/2,2,3);
        });
        c_stickr.addEventListener('touchmove', function(event) {
          event.preventDefault();
          touches = event.touches;
          window.requestAnimationFrame(update);
        });
        c_stickr.addEventListener('touchstart', function(event) {
          console.log('startr');
          end=0;
        });
        navigator.vibrate = navigator.vibrate || navigator.webkitVibrate || navigator.mozVibrate || navigator.msVibrate;
        window.addEventListener("gamepadconnected", function(e) { gamepadHandler(e, true); }, false);
        window.addEventListener("gamepaddisconnected", function(e) { gamepadHandler(e, false); }, false);
      };

      function getVBat(){
        if(wsconnect){
          console.log("get");
          connection.send("get");
        }
      }
  
      function Button1Change(checkbox)
      {
        if(checkbox.checked==true)
          ppm[4]=1800;
        else
          ppm[4]=1100;
        window.requestAnimationFrame(update);
        console.log("Button1: " + ppm[4]);
      }
      function Button2Change(checkbox)
      {
        if(checkbox.checked==true)
          ppm[5]=1800;
        else
          ppm[5]=1100;
        window.requestAnimationFrame(update);
        console.log("Button2: " + ppm[5]);
      }
      function Button3Change(checkbox)
      {
        if(checkbox.checked==true)
          ppm[6]=1800;
        else
          ppm[6]=1100;
        window.requestAnimationFrame(update);
        console.log("Button3: " + ppm[6]);
      }
       function Button4Change(checkbox)
      {
        if(checkbox.checked==true)
          ppm[7]=1800;
        else
          ppm[7]=1100;
        window.requestAnimationFrame(update);
        console.log("Button4: " + ppm[7]);
      }
  
      function resize()
      {
        ctx_left.canvas.height=window.innerHeight-(window.innerHeight/10*2);
        ctx_left.canvas.width=(window.innerWidth-(window.innerWidth/10*2))/2;
  
        ctx_right.canvas.height=window.innerHeight-(window.innerHeight/10*2);
        ctx_right.canvas.width=(window.innerWidth-(window.innerWidth/10*2))/2;
  
        ctx_stickl.canvas.height=ctx_left.canvas.height;
        ctx_stickl.canvas.width=ctx_left.canvas.width;
  
        ctx_stickr.canvas.height=ctx_right.canvas.height;
        ctx_stickr.canvas.width=ctx_right.canvas.width;
  
        document.getElementById("trim1x").min=-(ctx_stickl.canvas.width/4);
        document.getElementById("trim1x").max=(ctx_stickl.canvas.width/4);
        document.getElementById("trim2x").min=-(ctx_stickr.canvas.width/4);
        document.getElementById("trim2x").max=(ctx_stickr.canvas.width/4);
        document.getElementById("trim1y").min=-(ctx_stickl.canvas.width/4);
        document.getElementById("trim1y").max=(ctx_stickl.canvas.width/4);
        document.getElementById("trim2y").min=-(ctx_stickr.canvas.width/4);
        document.getElementById("trim2y").max=(ctx_stickr.canvas.width/4);
  
        draw_background(ctx_left);
        draw_background(ctx_right);
        draw_stick(ctx_stickl,ctx_stickl.canvas.width/2,ctx_stickl.canvas.height);
        draw_stick(ctx_stickr,ctx_stickr.canvas.width/2,ctx_stickr.canvas.height/2);
  
      }
  
      function draw_stick(context,x,y,ppm0,ppm1)
      {
        context.clearRect(0, 0, context.canvas.width, context.canvas.height);
  
            context.beginPath();
              context.arc(x,y,window.innerWidth/100*2,0,2*Math.PI);
              if(wsconnect)
                context.fillStyle = 'green';
              else
                context.fillStyle = 'red';
              context.fill();
              context.lineWidth = 5;
              context.strokeStyle = '#003300';
              context.stroke();
  
        ppm[ppm0] = parseInt(1000+((1000/context.canvas.width)*x))
        ppm[ppm1] = parseInt(2000-((1000/context.canvas.height)*y))
      }
  
  
      function draw_background(ctx)
      {
               ctx.beginPath();
               for(var i=0;i<ctx.canvas.width/2;i+=ctx.canvas.width/20)
               {
                       ctx.moveTo(ctx.canvas.width/2+i,ctx.canvas.height/2);
                       ctx.arc(ctx.canvas.width/2,ctx.canvas.height/2,i,0,2*Math.PI);
               }
               ctx.moveTo(0,ctx.canvas.height/2);
               ctx.lineTo(ctx.canvas.width,ctx.canvas.height/2);
               ctx.moveTo(ctx.canvas.width/2,0);
               ctx.lineTo(ctx.canvas.width/2,ctx.canvas.height);
               ctx.stroke();
      };
  
      function gamepadHandler(event, connecting) {
        var gamepad = event.gamepad;
   
    // Note:
    // gamepad === navigator.getGamepads()[gamepad.index];
  
        if (connecting) {
          gamepads[gamepad.index] = gamepad;
          console.log("Joystick connected " + gamepad.index);
          document.getElementById("invert1div").style.display="block";
          document.getElementById("invert2div").style.display="block";
          document.getElementById("trimdiv").style.display="block";
          window.requestAnimationFrame(update);
        } else {
          console.log("Joystick disconnect");
          delete gamepads[gamepad.index];
          document.getElementById("invert1div").style.display="none";
          document.getElementById("invert2div").style.display="none";
          document.getElementById("trimdiv").style.display="none";
        }
      }
  
      function checkButton(index){
          if(gamepads[0].buttons[index].value && !buttons[index])
          {
            buttons[index]=1;
            console.log("Button" + index);
            if(!document.getElementById("Button"+index).checked)
            {
              document.getElementById("Button"+index).checked = true;
              ppm[4+index]=1800;
            }
            else
            {
              document.getElementById("Button"+index).checked = false;
              ppm[4+index]=1100;
            }
          }
          if(!gamepads[0].buttons[index].value)
            buttons[index]=0;   
        }
  
  
      function update() {
        var nw = window.innerWidth;
        var nh = window.innerHeight;
        if ((w != nw) || (h != nh)) {
          w = nw;
          h = nh;
          resize();
        }
        for(var i=0;i<8;i++){
          if(ppm[i]!=oldppm[i])
          {
            oldppm[i]=ppm[i];
            var sendframe=new Uint8Array(3);
            sendframe[0]=i;
            sendframe[1]=ppm[i]>>8;
            sendframe[2]=ppm[i];
            if(wsconnect)
              connection.send(sendframe);
          }
        }
        if(gamepads[0])
        {
          var pady0=(gamepads[0].axes[1]*ctx_stickl.canvas.height/2);
          var pady1=(gamepads[0].axes[3]*ctx_stickr.canvas.height/2);
          if(document.getElementById("invert1").checked)
            pady0=-pady0;
          if(document.getElementById("invert2").checked)
            pady1=-pady1;
          draw_stick(ctx_stickl,parseInt(document.getElementById("trim1x").value)+((ctx_stickl.canvas.width/2)+(gamepads[0].axes[0]*ctx_stickl.canvas.width/2)),(ctx_stickl.canvas.height/2)+pady0-parseInt(document.getElementById("trim1y").value),0,1);
          draw_stick(ctx_stickr,parseInt(document.getElementById("trim2x").value)+(ctx_stickr.canvas.width/2)+(gamepads[0].axes[2]*ctx_stickr.canvas.width/2),(ctx_stickr.canvas.height/2)+pady1-parseInt(document.getElementById("trim2y").value),2,3);
          checkButton(0);
          checkButton(1);
          checkButton(2);
          checkButton(3);
          window.requestAnimationFrame(update);
        }
        else
        {
          var i, len = touches.length;
          var left=0;
          var right=0;
          for (i=0; i<len; i++) {
            var touch = touches[i];
            var px = touch.pageX-touch.target.offsetLeft;
            var py = touch.pageY-touch.target.offsetTop;
            console.log(touch.target.id);
            if(touch.target.id=="Canvas_stickl" && !left)
            {
              if(px>ctx_stickl.canvas.width)
                px=ctx_stickl.canvas.width;
              if(py<0)
                py=0;
              if(px<0)
                px=0;
              if(py>ctx_stickl.canvas.height)
                py=ctx_stickl.canvas.height;
              draw_stick(ctx_stickl,px,py,0,1);
              left=1;
            }
            if(touch.target.id=="Canvas_stickr" && !right && !end)
            {
              if(px>ctx_stickr.canvas.width)
                px=ctx_stickr.canvas.width;
              if(py<0)
                py=0;
              if(px<0)
                px=0;
              if(py>ctx_stickr.canvas.height)
                py=ctx_stickr.canvas.height;
              draw_stick(ctx_stickr,px,py,2,3);
              right=1;
            }
          }
        }
      }
      function sendalive(){
        var sendframe=new Uint8Array(3);

        sendframe[0]=0;
        sendframe[1]=ppm[0]>>8;
        sendframe[2]=ppm[0];
        if(wsconnect)
          connection.send(sendframe);

      }
      </script>
  
  
      </body>
      </html>
      )=====";
