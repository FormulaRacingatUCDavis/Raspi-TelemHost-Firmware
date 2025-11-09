import { ResponsiveContainer, CartesianGrid, Line, LineChart, XAxis, YAxis } from 'recharts';
import GraphFilterSidebar from './Options';
import './Graph.css';

interface Props {
  title: string;
  data: { timestamp: number; value: number }[];
}

const Graph = ({ title, data }: Props) => {
  return (
    <div className='graphWrapper bg-white rounded-4 shadow-sm p-4'>
      <GraphFilterSidebar title={title} />
      <div className='graphContent'>
        <h6 className='text-center'>{title}</h6>
        <ResponsiveContainer>
          <LineChart data={data}>
            <Line
              type='monotone'
              dataKey='value'
              stroke='#2196F3'
              strokeWidth={3}
            />
            <CartesianGrid strokeDasharray='3 3' />
            <XAxis dataKey='timestamp' />
            <YAxis />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
};

export default Graph;