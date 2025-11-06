import FRUCDLogo from './assets/FRUCDLogo.svg';

function Navbar() {
  return (
    <nav className="navbar navbar-expand-lg navbar-light bg-light">
      <a className="navbar-brand d-flex align-items-center" href="#">
        <img
          src={FRUCDLogo}
          alt="FRUCD Logo"
          className="d-inline-block align-text-top me-2"
          style={{
            height: '3.5rem',
            width: 'auto',
            objectFit: 'contain',
            marginTop: '-0.25rem',
          }}
        />
      </a>
      <ul className="navbar-nav me-auto mb-2 mb-lg-0">
        <li className="nav-item">
          <a className="nav-link active" href="#">View Camera</a>
        </li>
        <li className="nav-item">
          <a className="nav-link active" href="#">Start Recording</a>
        </li>
        <li className="nav-item dropdown">
          <a
            className="nav-link dropdown-toggle"
            href="#"
            role="button"
            data-bs-toggle="dropdown"
            aria-expanded="false"
          >
            Export CSV
          </a>
          <ul className="dropdown-menu">
            <li><a className="dropdown-item" href="#">Example CSV</a></li>
          </ul>
        </li>
      </ul>
    </nav>
  );
}

export default Navbar;