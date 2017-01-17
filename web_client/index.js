/**
 * Created by Weicheng Huang on 2017/1/16.
 */

var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var SerialPort = require("serialport");
var serialport;
var charList = ['a','b','c','d','e','f','g','h','i','j','k','l'];

app.get('/', function (req, res) {
    res.sendfile('index.html');
});
var closed = true;
io.on('connection', function (socket) {
    console.log('a user connected');
    client = socket;

    if (serialport) {
        serialport.close(function () {
            closed = true;
        });
        serialport = null;
    }

    serialport = new SerialPort("COM6", {"baudRate": 115200, parser: SerialPort.parsers.readline("\n")});
    serialport.on('open', function () {
        console.log('Serial Port Opend');
        serialport.on('data', function (data) {
            if(data !=""){
                console.log("Serial Receive:" + data);
                socket.emit("message", data);
            }

        });
    });
    socket.on("reply",function (e) {
        var c = charList[e[0] - 1] +":"+charList[e[1] - 1];
        console.log("Serial Write:" + c + "$");
        serialport.write(c + "$");
    });
});

http.listen(3000, function () {
    console.log('listening on *:3000');
});


