import React, { Fragment } from 'react';
import { Route, Switch } from 'react-router-dom';
import AppWithSerialCommunication from '../Home';
import NoMatch from '../NoMatch';

function AppRoutes() {
  return (
    <Fragment>
      <Switch>
        <Route exact path="/" component={AppWithSerialCommunication} />
        <Route component={NoMatch} />
      </Switch>
    </Fragment>
  );
}

export default AppRoutes;
