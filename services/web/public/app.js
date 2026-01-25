import '@picocss/pico'
import Chart from 'chart.js/auto';

(async function() {
  const data = {
    datasets: [{
      label: 'Motor Speed',
      data: [
        {x: 0, y: 10 },
        {x: 10, y: 5},
        {x: 0.5, y: 5.5}
      ],
      backgroundColor: 'rgb(0, 0, 0)'
    }],
  };

  new Chart(
    document.getElementById('motor_speed'),
    {
    type: 'scatter',
    data: data,
    options: {
      scales: {
        x: {
          type: 'linear',
          position: 'bottom',
          title: {
            display: true,
            text: 'Time [s]',
            font: {
              size: 16
            }
          }
        },
        y: {
          beginAtZero: true,
          title: {
            display: true,
            text: 'Motor Speed [RPM]',
            font: {
              size: 16
            }
          }
        }
      }
    }
  });
})();