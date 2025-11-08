import React, { useState } from 'react';

function GraphFilterSidebar({ onFilterChange, initialFilters }) {
  // Default filters if none provided
  const defaultFilters = {
    mcTempA: true,
    mcTempB: true,
    mcTempC: true,
    hiTemp: true,
    torque: true
  };

  const [activeFilters, setActiveFilters] = useState(initialFilters || defaultFilters);

  const toggleFilter = (filter) => {
    const newFilters = {
      ...activeFilters,
      [filter]: !activeFilters[filter]
    };
    setActiveFilters(newFilters);
    
    // Call the parent's callback function if provided
    if (onFilterChange) {
      onFilterChange(newFilters);
    }
  };

  return (
    <div style={styles.sidebar}>
      {/* INV Section */}
      <div style={styles.filterSection}>
        <div style={styles.sectionLabel}>INV</div>
        <button 
          style={{
            ...styles.filterButton,
            ...(activeFilters.mcTempA ? styles.filterButtonActive : {})
          }}
          onClick={() => toggleFilter('mcTempA')}
        >
          <span style={styles.checkbox}>{activeFilters.mcTempA ? '☑' : '☐'}</span>
          MC Temp A
        </button>
        <button 
          style={{
            ...styles.filterButton,
            ...(activeFilters.mcTempB ? styles.filterButtonActive : {})
          }}
          onClick={() => toggleFilter('mcTempB')}
        >
          <span style={styles.checkbox}>{activeFilters.mcTempB ? '☑' : '☐'}</span>
          MC Temp B
        </button>
        <button 
          style={{
            ...styles.filterButton,
            ...(activeFilters.mcTempC ? styles.filterButtonActive : {})
          }}
          onClick={() => toggleFilter('mcTempC')}
        >
          <span style={styles.checkbox}>{activeFilters.mcTempC ? '☑' : '☐'}</span>
          MC Temp C
        </button>
      </div>

      {/* PEI Section */}
      <div style={styles.filterSection}>
        <div style={styles.sectionLabel}>PEI</div>
        <button 
          style={{
            ...styles.filterButton,
            ...(activeFilters.hiTemp ? styles.filterButtonActive : {})
          }}
          onClick={() => toggleFilter('hiTemp')}
        >
          <span style={styles.checkbox}>{activeFilters.hiTemp ? '☑' : '☐'}</span>
          HI Temp
        </button>
      </div>

      {/* VCU Section */}
      <div style={styles.filterSection}>
        <div style={styles.sectionLabel}>VCU</div>
        <button 
          style={{
            ...styles.filterButton,
            ...(activeFilters.torque ? styles.filterButtonActive : {})
          }}
          onClick={() => toggleFilter('torque')}
        >
          <span style={styles.checkbox}>{activeFilters.torque ? '☑' : '☐'}</span>
          Torque
        </button>
      </div>
    </div>
  );
}

const styles = {
  sidebar: {
    minWidth: '200px',
    maxWidth: '200px',
    display: 'flex',
    flexDirection: 'column',
    gap: '16px'
  },
  filterSection: {
    display: 'flex',
    flexDirection: 'column',
    gap: '6px'
  },
  sectionLabel: {
    fontSize: '13px',
    fontWeight: '700',
    color: '#1a1a1a',
    marginBottom: '4px',
    letterSpacing: '0.5px'
  },
  filterButton: {
    display: 'flex',
    alignItems: 'center',
    gap: '8px',
    padding: '8px 12px',
    border: 'none',
    borderRadius: '4px',
    backgroundColor: 'transparent',
    cursor: 'pointer',
    fontSize: '14px',
    fontWeight: '400',
    color: '#333',
    textAlign: 'left',
    transition: 'all 0.2s ease',
    fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif'
  },
  filterButtonActive: {
    backgroundColor: '#fff9e6',
    fontWeight: '500'
  },
  checkbox: {
    fontSize: '16px',
    color: '#ffd500'
  }
};

export default GraphFilterSidebar;
