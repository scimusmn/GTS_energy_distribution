import React, { Component } from 'react';
import { Container } from 'reactstrap';
import propTypes from 'prop-types';

import { WAKE_ARDUINO } from '../Arduino/arduino-base/ReactSerial/arduinoConstants';
import IPC from '../Arduino/arduino-base/ReactSerial/IPCMessages';
import withSerialCommunication from '../Arduino/arduino-base/ReactSerial/SerialHOC';

const initialState = {
  CoalBurners: 0,
  GasBurners: 0,
  HydroDams: 0,
  WindFarms: 0,
  SolarFarms: 0,
  total: 0,
  pingArduinoStatus: false,
  refreshPortCount: 0,
};

class App extends Component {
  constructor(props) {
    super(props);
    this.state = initialState;
    this.handleSubmit = this.handleSubmit.bind(this);
    this.handleChange = this.handleChange.bind(this);
  }

  componentDidMount() {
    const { setOnDataCallback } = this.props;
    setOnDataCallback(this.onSerialData);
    document.addEventListener('keydown', this.handleReset);
    this.pingArduino();
  }

  onSerialData(data) {
    const { handshake } = this.state;

    if (data.message === 'arduino-ready') {
      if (!handshake) this.setState({ handshake: true });

      this.setState({
        pingArduinoStatus: false,
        refreshPortCount: 0,
      });
    }
  }

  handleChange(e) {
    e.preventDefault();
  }

  handleSubmit() {
    const {
      CoalBurners, GasBurners, HydroDams, WindFarms, SolarFarms,
    } = this.state;

    const etotal = CoalBurners + GasBurners + HydroDams + WindFarms + SolarFarms;
    this.setState({ total: etotal });
  }

  pingArduino() {
    const { sendData } = this.props;
    const { pingArduinoStatus } = this.state;

    if (pingArduinoStatus) this.refreshPorts();
    this.setState({ pingArduinoStatus: true });
    sendData(WAKE_ARDUINO);

    setTimeout(() => { this.pingArduino(); }, 5000);
  }

  refreshPorts() {
    const { sendData, startIpcCommunication, stopIpcCommunication } = this.props;
    const { refreshPortCount } = this.state;

    if (refreshPortCount === 2) {
      this.setState({ handshake: false });

      console.log('sending RESET-PORT');
      sendData(IPC.RESET_PORTS_COMMAND);
      console.log('restarting ipcCommunication...');

      stopIpcCommunication();
      startIpcCommunication();
    }
  }

  render() {
    const {
      CoalBurners, GasBurners, HydroDams, WindFarms, SolarFarms, total,
    } = this.state;

    return (
      <Container>
        <form onSubmit={this.handleSubmit}>
          <label htmlFor="CoalBurners">
            Coal Burners:
            <input
              min="0"
              type="number"
              value={CoalBurners}
              onChange={this.handleChange}
              id="CoalBurners"
            />
          </label>
          <label htmlFor="GasBurners">
            Gas Burners:
            <input
              min="0"
              type="number"
              value={GasBurners}
              onChange={this.handleChange}
              id="GasBurners"
            />
          </label>
          <label htmlFor="HydroDams">
            Hydro Dams:
            <input
              min="0"
              type="number"
              value={HydroDams}
              onChange={this.handleChange}
              id="HydroDams"
            />
          </label>
          <label htmlFor="WindFarms">
            Wind Farms:
            <input
              min="0"
              type="number"
              value={WindFarms}
              onChange={this.handleChange}
              id="WindFarms"
            />
          </label>
          <label htmlFor="SolarFarms">
            Solar Farms:
            <input
              min="0"
              type="number"
              value={SolarFarms}
              onChange={this.handleChange}
              id="SolarFarms"
            />
          </label>
          <input type="submit" value="Submit" />
        </form>
        <p>
          {total}
        </p>
      </Container>
    );
  }
}

App.propTypes = {
  sendData: propTypes.func.isRequired,
  setOnDataCallback: propTypes.func.isRequired,
  startIpcCommunication: propTypes.func.isRequired,
  stopIpcCommunication: propTypes.func.isRequired,
};

const AppWithSerialCommunication = withSerialCommunication(App);
export default AppWithSerialCommunication;
