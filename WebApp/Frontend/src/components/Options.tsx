import './Options.css';

interface Props {
  title: string;
}

const Options = ({ title }: Props) => {
  return (
    <div className='sidebar'>
      <div className='overflow-y-auto'>
        <div className='filterSection'>
          <h6 className='text-start'>INV</h6>
            <div className="form-check">
              <input className="form-check-input" type="radio" name={`${title}`} id={`${title}-1`} />
              <label className="form-check-label" htmlFor={`${title}-1`}>
                MC Temp A
              </label>
            </div>
            <div className="form-check">
              <input className="form-check-input" type="radio" name={`${title}`} id={`${title}-2`} />
              <label className="form-check-label" htmlFor={`${title}-2`}>
                MC Temp B
              </label>
            </div>
            <div className="form-check">
              <input className="form-check-input" type="radio" name={`${title}`} id={`${title}-3`} />
              <label className="form-check-label" htmlFor={`${title}-3`}>
                MC Temp C
              </label>
            </div>
        </div>
        <div className='filterSection'>
          <h6 className='text-start'>PEI</h6>
            <div className="form-check">
              <input className="form-check-input" type="radio" name={`${title}`} id="radio2" />
              <label className="form-check-label" htmlFor="radio2">
                HI Temp
              </label>
            </div>
        </div>
        <div className='filterSection'>
          <h6 className='text-start'>VCU</h6>
            <div className="form-check">
              <input className="form-check-input" type="radio" name={`${title}`} id="radio3" />
              <label className="form-check-label" htmlFor="radio3">
                Torque
              </label>
            </div>
        </div>
      </div>
    </div>
  );
}

export default Options;