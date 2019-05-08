var socket = new WebSocket('ws://' + location.hostname + ':81/', ['arduino']);
//var socket = new WebSocket('ws://192.168.0.9:81/', ['arduino']);
socket.binaryType = 'arraybuffer';
socket.onopen = function () {
  socket.send('Connect ' + new Date());
};
socket.onerror = function (error) {
  console.log('WebSocket Error ', error);
};
socket.onmessage = function (e) {
  console.log('Server: ', e.data);
  if(e.data == 'LED On') {
    $('#led-status').text('ON')
  }
  if(e.data == 'LED Off') {
    $('#led-status').text('OFF')
  }
};
socket.onclose = function () {
  console.log('WebSocket connection closed');
};