const express = require('express');
const app = express();
const bodyParser = require('body-parser');
const path = require('path');

function extractIPv4FromIPv6(ipv6Address) {
  if (ipv6Address.startsWith('::ffff:')) {
    return ipv6Address.split(':').pop(); 
  }
  return ipv6Address; 
}

let machineIp;

app.use(bodyParser.json());

app.use((req, res, next) => {
  req.setTimeout(20 * 60 * 1000);  // 20 minutes
  next();
});

app.use(express.static(path.join(__dirname, 'public')));

let machineStatus = 0;

app.get('/status', (req, res) => {

  res.json({ status: machineStatus, ip: machineIp });
});


app.post('/update-status', (req, res) => {

  const newStatus = req.body.status;

  machineStatus = newStatus;

  const IP = req.ip;

  machineIp = extractIPv4FromIPv6(IP);

  console.log("machineIp = "+ machineIp);
  res.json({IP: machineIp});
});

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

app.get('/script.js', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'script.js'));
});

const PORT = process.env.PORT || 80;
app.listen(PORT, '0.0.0.0', () => {
  console.log(`Server is running on http://0.0.0.0:${PORT}`);
});