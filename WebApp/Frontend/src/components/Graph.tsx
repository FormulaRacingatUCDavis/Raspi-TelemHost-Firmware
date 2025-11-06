import { ResponsiveContainer, CartesianGrid, Line, LineChart, XAxis, YAxis } from "recharts";

type Props = {
  title: string;
  data: { timestamp: number; value: number }[];
};

const Graph = ({ title, data }: Props) => {
  return (
    <div className="bg-white rounded-4 shadow-sm p-4" style={{ height: "350px" }}>
      <h6 className="fw-semibold mb-3 text-center">{title}</h6>
      <ResponsiveContainer width="100%" height="90%">
        <LineChart data={data}>
          <Line type="monotone" dataKey="value" stroke="#2196F3" strokeWidth={3} />
          <CartesianGrid strokeDasharray="3 3" />
          <XAxis dataKey="timestamp" />
          <YAxis />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
};

export default Graph;