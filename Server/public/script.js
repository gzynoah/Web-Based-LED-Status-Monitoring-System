function fetchMachineStatus() {
    fetch('/status')
        .then(response => response.json())
        .then(data => {
            const clientIP = data.ip; 
            const ipCells = document.querySelectorAll('[data-ip]');
            let statusCellFound = false;

            ipCells.forEach(ipCell => {
                if (ipCell.getAttribute('data-ip') === clientIP) {
                    const statusCell = ipCell.parentElement.querySelector('#status-cell');
                    if (statusCell) {
                        statusCellFound = true;
                        if (data.status === 0) {
                            statusCell.classList.remove('red-square');
                            statusCell.classList.add('green-square');
                        } else {
                            statusCell.classList.remove('green-square');
                            statusCell.classList.add('red-square');
                        }
                    }
                }
            });

            if (!statusCellFound) {
                console.error(`Status cell not found for IP address: ${clientIP}`);
            }
        })
        .catch(error => console.error('Error fetching machine status:', error));
}

fetchMachineStatus();

setInterval(fetchMachineStatus, 2000);
