// Simplified JavaScript for frontend
const API_URL = window.location.origin + '/api/esp32-data';

document.addEventListener('DOMContentLoaded', function() {
  loadCurrentData();
  document.getElementById('saveBtn').addEventListener('click', saveData);
  document.getElementById('refreshBtn').addEventListener('click', loadCurrentData);
});

function loadCurrentData() {
  fetch(API_URL).then(r => r.json()).then(data => {
    document.getElementById('temp1').value = data.temp1;
    document.getElementById('temp2').value = data.temp2;
    document.getElementById('status').value = data.status;
    document.getElementById('custom').value = data.custom;
    updateDisplay(data);
  }).catch(e => showMessage('Error: ' + e, 'error'));
}

function saveData() {
  const data = {
    temp1: document.getElementById('temp1').value,
    temp2: document.getElementById('temp2').value,
    status: document.getElementById('status').value,
    custom: document.getElementById('custom').value
  };
  fetch(API_URL, {method: 'POST', headers: {'Content-Type': 'application/json'}, body: JSON.stringify(data)})
    .then(r => r.json()).then(r => {if(r.success) { updateDisplay(r.data); showMessage('Saved!', 'success'); } else showMessage('Error: ' + r.error, 'error');}).catch(e => showMessage('Error: ' + e, 'error'));
}

function updateDisplay(data) {
  document.getElementById('displayTemp1').textContent = data.temp1;
  document.getElementById('displayTemp2').textContent = data.temp2;
  document.getElementById('displayStatus').textContent = data.status;
  document.getElementById('displayCustom').textContent = data.custom;
}

function showMessage(msg, type) {
  const el = document.getElementById('message');
  el.textContent = msg;
  el.className = 'message ' + type;
  setTimeout(() => el.textContent = '', 4000);
}
