function control() {
  console.log('You are on Control page');  
}

registerPage('control', control);

function turnOn() {
  console.log('ON');
  connection.send('on');
}

function turnOff() {
  console.log('OFF');
  connection.send('off');
}