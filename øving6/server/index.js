"use strict";

const net = require('net');
const crypto = require("crypto");

const generateAcceptValue = acceptKey =>
  crypto
    // Creating sha1 hash function
    .createHash("sha1")
    .update(acceptKey + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", "binary")
    .digest("base64");

const httpServer = net.createServer((connection) => {
  connection.on('data', () => {
    // Sending pure html Code to server
    let content = `<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
  </head>
  <body>
    <script>
    let ws = new WebSocket('ws://localhost:3001');
    ws.onmessage = event => document.getElementById("ok").innerHTML = document.getElementById("ok").innerHTML + "</br> Friend: "+ event.data;
    const msg = () => {
      let v = document.getElementById("melding").value
      ws.send(v);
      document.getElementById("ok").innerHTML = document.getElementById("ok").innerHTML + "</br> Me: "+ v;
    }
    </script>
    <h1>WebSocket test page</h1>
    <label for="lname">Skriv melding:</label>
    <input type="text" id="melding" name="melding"><br><br>
    <input type="submit" value="Submit" onclick="return msg();">
    <div id="ok" class="div">
    </div>
    <div id="canvas">
        <canvas id="tegnebrett" width="200" height="100" style="border:1px solid #000000></canvas>
    </div>
  </body>
</html>
`;
    connection.write('HTTP/1.1 200 OK\r\nContent-Length: ' + content.length + '\r\n\r\n' + content);
  });
});
httpServer.listen(3000, () => {
  console.log('HTTP server listening on port 3000');
});

// Array to handle multiple clients
let clients = [];
const wsServer = net.createServer((connection) => {
    
  console.log('Client connected');

  connection.on('data', data => {
    if(data.toString()[0] == "G") {
        // Key is generated from data 'Sec-WebSocket-Key: toTjWN1bpNz0ovaFNiBZBA==''
        var key = data.toString().substring(data.toString().indexOf("-Key: ") + 6, data.toString().indexOf("==") + 2);

        // Generating acceptvalue based on key
        var acceptValue = generateAcceptValue(key);

        const respons = [
            "HTTP/1.1 101 Web Socket Protocol Handshake",
            "Upgrade: websocket",
            "Connection: Upgrade", 
            "Sec-WebSocket-Accept:" + acceptValue
        ];
        connection.write(respons.join("\r\n") + "\r\n\r\n");
        clients.push(connection);
        console.log("Handshake complete")
        // If not handshake, code to handle messages
    } else {
        let message = "";
        let length = data[1] & 127;
        let maskStart = 2;
        let dataStart = maskStart + 4;
        for(let i = dataStart; i < dataStart + length; i++) {
            let byte = data[i] ^ data[maskStart + ((i - dataStart) % 4)];
            message += String.fromCharCode(byte);
        }
        console.log(message);
        sendMessage(message, connection);
    }
  });

  const sendMessage = (message, c) => {
    let buffer = Buffer.concat([
      new Buffer.from([
        0x81,
        "0x" +
          (message.length + 0x10000)
            .toString(16)
            .substr(-2)
            .toUpperCase()
      ]),
      Buffer.from(message)
    ]);
    console.log(buffer);
    clients.forEach(client => {
      if (c != client) client.write(buffer);
    });
  };

  connection.on('end', () => {
    console.log('Client disconnected');
  });
});
wsServer.on('error', (error) => {
  console.error('Error: ', error);
});
wsServer.listen(3001, () => {
  console.log('WebSocket server listening on port 3001');
});