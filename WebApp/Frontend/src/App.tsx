import 'bootstrap/dist/css/bootstrap.min.css';
import 'bootstrap/dist/js/bootstrap.bundle.min.js';
import Navbar from './components/Navbar';
import Graph from './components/Graph';
import './App.css';

const torque_data = [
  { timestamp: 0, value: 123 },
  { timestamp: 1, value: 213 },
  { timestamp: 2, value: 312 },
  { timestamp: 3, value: 231 },
  { timestamp: 4, value: 278 },
  { timestamp: 5, value: 329 },
];

const temperature_data = [
  { timestamp: 0, value: 65 },
  { timestamp: 1, value: 42 },
  { timestamp: 2, value: 53 },
  { timestamp: 3, value: 61 },
  { timestamp: 4, value: 45 },
  { timestamp: 5, value: 56 },
];

function App() {
  return (
    <>
      <Navbar />
      <div className="py-4 px-4">
        <div className="d-flex flex-column gap-4">
          <Graph title="Torque [Nm] vs Time [s]" data={torque_data} />
          <Graph title="Temperature [C] vs Time [s]" data={temperature_data} />
        </div>
      </div>
    </>
  );
}

export default App;