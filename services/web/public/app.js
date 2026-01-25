import '@picocss/pico'
import Chart from 'chart.js/auto';
import 'chartjs-adapter-moment';
import mqtt from 'mqtt';

const client = mqtt.connect('ws://192.168.137.249:9001');

let scatterChart;

client.on('connect', () => {
  console.log('Connected to MQTT broker.');
  client.subscribe('can/C0');
});

const MAX_POINTS = 1000;

client.on('message', (topic, message) => {
  if (topic === 'can/C0') {
    const data = JSON.parse(message.toString());

    if (scatterChart) {
      const dataset = scatterChart.data.datasets[0].data;
      
      dataset.push({ x: data.timestamp, y: data.dashboard_torque });

      if (dataset.length > MAX_POINTS) {
        dataset.splice(0, dataset.length - MAX_POINTS);
      }

      scatterChart.update('none');
    }
  }
});

(async function() {
  const data = {
    datasets: [{
      label: 'No Data Selected',
      data: [],
      backgroundColor: 'rgb(135, 139, 219)'
    }],
  };

  scatterChart = new Chart(
    document.getElementById('scatterChart'),
    {
    type: 'line',
    data: data,
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
            font: { size: 16 }
          }
        },
        y: {
          beginAtZero: true,
          title: {
            display: true,
            text: 'No Data Selected',
            font: { size: 16 }
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
})();