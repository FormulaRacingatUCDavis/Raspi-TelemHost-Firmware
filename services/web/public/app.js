import '@picocss/pico'
import Chart from 'chart.js/auto';
import 'chartjs-adapter-moment';
import mqtt from 'mqtt';

/* Connect to MQTT broker from web browser using WebSockets */

const client = mqtt.connect('ws://192.168.137.249:9001');

client.on('connect', () => {
  console.log('Connected to MQTT broker.');
  client.subscribe('can/A0');
  client.subscribe('can/C0');
});

/* Graph live data */

let lineChart;
let barChart;

const MAX_POINTS = 1000;

client.on('message', (topic, message) => {
  if (topic === 'can/A0') {
    const msg = JSON.parse(message.toString());

    if (barChart) {
      const dataset = barChart.data.datasets[0].data;

      dataset[0] = msg.inv_module_a_temp;
      dataset[1] = msg.inv_module_b_temp;
      dataset[2] = msg.inv_module_c_temp;

      barChart.update();
    }
  }
  else if (topic === 'can/C0') {
    const msg = JSON.parse(message.toString());

    if (lineChart) {
      const dataset = lineChart.data.datasets[0].data;
      
      dataset.push({ x: msg.timestamp, y: msg.dashboard_torque });

      if (dataset.length > MAX_POINTS) {
        dataset.splice(0, dataset.length - MAX_POINTS);
      }

      lineChart.update('none');
    }
  }
});

(async function() {
  const lineData = {
    datasets: [{
      label: 'No Data Selected',
      data: [],
      backgroundColor: 'rgb(135, 139, 219)'
    }],
  };

  lineChart = new Chart(
    document.getElementById('lineChart'),
    {
    type: 'line',
    data: lineData,
    options: {
      parsing: false,
      normalized: true,
      scales: {
        x: {
          ticks: {
            sampleSize: 10
          },
          type: 'time',
          time: {
            unit: 'second'
          },
          title: {
            display: true,
            text: 'Time',
          }
        },
        y: {
          beginAtZero: true,
          title: {
            display: true,
            text: 'No Data Selected',
          }
        }
      },
      decimation: {
        enabled: true,
        algorithm: 'min-max',
        samples: 1000
      }
    }
  });

  const labels = ['Module A', 'Module B', 'Module C'];
  const data = {
    labels: labels,
    datasets: [{
      label: 'Inverter Module Temperatures',
      data: [null, null, null],
      backgroundColor: [
        'rgba(255, 99, 132, 0.2)',
        'rgba(255, 159, 64, 0.2)',
        'rgba(255, 205, 86, 0.2)',
      ],
      borderColor: [
        'rgb(255, 99, 132)',
        'rgb(255, 159, 64)',
        'rgb(255, 205, 86)',
      ],
      borderWidth: 1
    }]
  };

  barChart = new Chart(
    document.getElementById('barChart'),
    {
      type: 'bar',
      data: data,
      options: {
        scales: {
          y: {
            beginAtZero: true,
            title: {
              display: true,
              text: 'Temperature [C]',
            }
          }
        }
      }
    }
  )

})();