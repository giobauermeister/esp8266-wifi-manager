function wificonfig() {
  console.log('You are on Wifi page');  
}
registerPage('wificonfig', wificonfig);

$('.connecting-spinner').hide();
$('.list-network-spinner').hide();
$('#connectBtn').prop('disabled', true);

// ON MESSAGE FROM SOCKET EVENT
var networkList = document.getElementById("ssidDropDown");
var ssidArray = [];
socket.onmessage = function (e) {
  console.log('Server: ', e.data);
  var ssidArray = JSON.parse(e.data);
  $('.list-network-spinner').hide();
  for (var i = 0; i < ssidArray.length; i++){
    console.log('creating dorpdown list');                
    var ssidName = ssidArray[i];  
    var a = document.createElement("a");
    a.className = 'dropdown-item';            
    var text = document.createTextNode(ssidName);
    a.appendChild(text);  
    networkList.appendChild(a);  
  }
  $('div.dropdown-menu a').on('click', function(e) {
    var ssid = $(this).text();
    $('#ssidInputField').val(ssid);
    console.log('Selected: ' + ssid);
  });
};

// HIDE AND SHOW PASSWORD FIELD
$(".reveal").on('click',function() {
  var $pwd = $(".pwd");
  if ($pwd.attr('type') === 'password') {
      $pwd.attr('type', 'text');
      $("#eye-icon").attr('src', 'icons/eye-slash-solid.svg');
  } else {
      $pwd.attr('type', 'password');
      $("#eye-icon").attr('src', 'icons/eye-solid.svg');
  }
});

// DROPDOWN CLICK EVENT
$(".dropdown-toggle").on("click", function(event){
  $('.connecting-spinner').hide();
  if ($('.dropdown').find('.dropdown-menu').is(":hidden")){
    $('.dropdown-item').remove();
    console.log('getting networks');
    var JsonData = {
      "messageInfo": {
        "message": "getNetworks",
        "size": null
      }
    };
    JsonData.messageInfo.size = JsonData.messageInfo.message.length.toString();
    socket.send(JSON.stringify(JsonData));
    $('.list-network-spinner').show();
  }
});

// CONECTION BUTTON CLICK EVENT
$('#connectBtn').on('click', function() {
  console.log('connecting to ' + $('#ssidInputField').val());
  $('#connectingLabel').text('connecting to ' + $('#ssidInputField').val());
  $('.connecting-spinner').removeAttr('hidden');
  $('.connecting-spinner').show();
  var JsonData = {
    "networkInfo": {
      "ssid": $('#ssidInputField').val(),
      "password": $('#passInputField').val()
    }
  };
  console.log('SSID: ' + JsonData.networkInfo.ssid);
  console.log('Pass: ' + JsonData.networkInfo.pass);
  console.log(JsonData);
  socket.send(JSON.stringify(JsonData));
  
});

// CHECK INPUT FIELDS TO ENABLE CONNECT BUTTON
$('#ssidInputField').on('keyup', function() {
  if($('#passInputField').val() != '' && $('#ssidInputField').val() != '') {
    $('#connectBtn').prop('disabled', false);
  } else if($('#passInputField').val() == '' || $('#ssidInputField').val() == '') {
    $('#connectBtn').prop('disabled', true);
  }
});
$('#passInputField').on('keyup', function() {
  if($('#ssidInputField').val() != '' && $('#passInputField').val() != '') {
    $('#connectBtn').prop('disabled', false);
  } else if($('#passInputField').val() == '' || $('#ssidInputField').val() == '') {
    $('#connectBtn').prop('disabled', true);
  }
});










