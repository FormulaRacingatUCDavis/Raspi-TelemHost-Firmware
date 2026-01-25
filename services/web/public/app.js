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

client.on('message', (topic, message) => {
  if (topic == 'can/C0') {
    const data = JSON.parse(message.toString());
    console.log("Time:", data.timestamp, "Speed:", data.dashboard_torque);

    if (scatterChart) {
      scatterChart.data.datasets[0].data.push({ x: data.timestamp, y: data.dashboard_torque });
      scatterChart.update('none');
    }
  }
});

(async function() {
  const data = {
    datasets: [{
      label: 'No Data Selected',
      data: [],
      backgroundColor: 'rgb(0, 0, 0)'
    }],
  };

  scatterChart = new Chart(
    document.getElementById('scatterChart'),
    {
    type: 'scatter',
    data: data,
    options: {
      animation: false,
      scales: {
        x: {
          type: 'time',
          time: {
            tooltipFormat: 'HH:mm:ss',
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
      }
    }
  });
})();