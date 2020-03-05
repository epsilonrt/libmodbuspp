/* Copyright © 2018-2019 Pascal JEAN, All rights reserved.
 * This file is part of the libmodbuspp Library.
 *
 * The libmodbuspp Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * The libmodbuspp Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the libmodbuspp Library; if not, see <http://www.gnu.org/licenses/>.
 */
#include <sstream>
#include "master_p.h"
#include "slave_p.h"
#include "config.h"

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                             Master Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Master (Master::Private &dd) : Device (dd) {}

  // ---------------------------------------------------------------------------
  Master::Master () :  Device (*new Private (this)) {}

  // ---------------------------------------------------------------------------
  Master::Master (Net net, const std::string & connection,
                  const std::string & settings) : Master () {
    PIMP_D (Master);

    d->setBackend (net, connection, settings);
  }

  // ---------------------------------------------------------------------------
  Master::Master (const std::string & jsonfile,
                  const std::string & key) : Master () {
    PIMP_D (Master);

    d->setConfigFromFile (jsonfile, key);
  }

  // ---------------------------------------------------------------------------
  Master::~Master() {}

  // ---------------------------------------------------------------------------
  bool Master::setRecoveryLink (bool recovery) {

    if (isValid()) {
      PIMP_D (Master);

      modbus_set_error_recovery (d->ctx(), recovery ?
                                 MODBUS_ERROR_RECOVERY_LINK :
                                 MODBUS_ERROR_RECOVERY_NONE);
      d->recoveryLink = recovery;
      return true;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  bool Master::hasSlave (int slaveAddr) const {
    PIMP_D (const Master);

    return d->slave.count (slaveAddr) > 0;
  }

  // ---------------------------------------------------------------------------
  Slave & Master::addSlave (int slaveAddr) {
    PIMP_D (Master);

    return *d->addSlave (slaveAddr);
  }

  // ---------------------------------------------------------------------------
  Slave & Master::slave (int slaveAddr) {
    PIMP_D (Master);

    return *d->slave.at (d->defaultSlave (slaveAddr));
  }

  // ---------------------------------------------------------------------------
  const Slave & Master::slave (int slaveAddr) const {
    PIMP_D (const Master);

    return *d->slave.at (d->defaultSlave (slaveAddr));
  }

  // ---------------------------------------------------------------------------
  Slave * Master::slavePtr (int slaveAddr) {
    PIMP_D (Master);
    int i = d->defaultSlave (slaveAddr);

    return hasSlave (i) ? d->slave.at (i).get() : nullptr;
  }

  // ---------------------------------------------------------------------------
  const Slave * Master::slavePtr (int slaveAddr) const {
    PIMP_D (const Master);
    int i = d->defaultSlave (slaveAddr);

    return hasSlave (i) ? d->slave.at (i).get() : nullptr;
  }

  // ---------------------------------------------------------------------------
  Slave & Master::operator[] (int slaveAddr) {
    PIMP_D (Master);

    return * d->slave.at (slaveAddr);
  }

  // ---------------------------------------------------------------------------
  const Slave & Master::operator[] (int slaveAddr) const {
    PIMP_D (const Master);

    return * d->slave.at (slaveAddr);
  }

  // ---------------------------------------------------------------------------
  const std::map <int, std::shared_ptr<Slave>> & Master::slaves() const {
    PIMP_D (const Master);

    return d->slave;
  }

  // ---------------------------------------------------------------------------
  int Master::sendRawRequest (const Request & req) {

    if (isValid()) {
      PIMP_D (Device);

      return modbus_send_raw_request (d->ctx(), req.adu(), req.aduSize());;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  int Master::receiveResponse (Response & resp) {

    if (isValid()) {
      PIMP_D (Device);

      int rc = modbus_receive_confirmation (d->ctx(), resp.adu());;
      if (rc > 0) {

        resp.setAduSize (rc);
      }
      return rc;
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  //
  //                         Master::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Master::Private::Private (Master * q) : Device::Private (q) {}

  // ---------------------------------------------------------------------------
  Master::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  // virtual
  void Master::Private::setBackend (Net net, const std::string & connection,
                                    const std::string & settings) {

    Device::Private::setBackend (net, connection, settings);
    switch (net) {

      case Tcp:
        (void) addSlave (TcpSlave);
        break;

      case Rtu:
        (void) addSlave (Broadcast);
        break;
    }
  }

  // ---------------------------------------------------------------------------
  // virtual
  void Master::Private::setConfig (const nlohmann::json & config) {
    PIMP_Q (Master);

    Json::setConfig (q, config);
  }

  // ---------------------------------------------------------------------------
  // virtual
  Slave * Master::Private::addSlave (int slaveAddr) {
    PIMP_Q (Master);

    if (q->isValid()) {

      if (modbus_set_slave (ctx(), slaveAddr) != 0) {
        std::ostringstream oss;

        oss << "Error: Unable to add slave[" << slaveAddr << "]\n" << lastError();
        throw std::invalid_argument (oss.str());
      }

      std::shared_ptr<Slave> s;

      if (slave.count (slaveAddr)) {

        s = slave[slaveAddr];
      }
      else {

        s = std::make_shared<Slave> (slaveAddr, q);
        slave[slaveAddr] = s;
      }
      return s.get();
    }
    throw std::runtime_error ("Error: backend not set !");
  }

  // ---------------------------------------------------------------------------
  //
  //                         Modbus::Json Namespace
  //
  // ---------------------------------------------------------------------------
  namespace Json {

    // -------------------------------------------------------------------------
    void setConfig (Master * master, const nlohmann::json & j) {

      setConfig (reinterpret_cast<Device *> (master), j);
      if (j.contains ("slaves")) {

        auto slaves = j["slaves"];
        for (const auto & config : slaves) {

          auto id = config["id"].get<int>();
          Slave & slv = master->addSlave (id);
          setConfig (&slv, config);
        }
      }
    }
  }

}
/* ========================================================================== */
