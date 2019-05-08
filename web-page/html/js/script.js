console.log('Hello World!');

$('#sendBtn').on('click', function() {
  var jsonData = {
    "networkInfo": {
      "ssid": $('#ssidInput').val(),
      "password": $('#passInput').val()
    }
  };
  console.log("SSID: " + jsonData.networkInfo.ssid + " | Password: " + jsonData.networkInfo.password);
  console.log(jsonData);
  socket.send(JSON.stringify(jsonData));
});

$('#sendMessageBtn').on('click', function() {
  var jsonData = {
    "messageInfo": {
      "message": $('#messageInput').val(),
      "size": $('#messageInput').val().length.toString() //converte o tamanho int em string para ArduinoJson reconhecer
    }
  };
  console.log("Message: " + jsonData.messageInfo.message + " | Length: " + jsonData.messageInfo.size);
  console.log(jsonData);
  socket.send(JSON.stringify(jsonData));
});

$('#ledOnBtn').on('click', function() {
  var jsonData = {
    "ioControlInfo": {
      "ioPin": "2",
      "ioState": "1"
    }
  };
  console.log(jsonData);
  socket.send(JSON.stringify(jsonData));
});

$('#ledOffBtn').on('click', function() {
  var jsonData = {
    "ioControlInfo": {
      "ioPin": "2",
      "ioState": "0"
    }
  };
  console.log(jsonData);
  socket.send(JSON.stringify(jsonData));
});