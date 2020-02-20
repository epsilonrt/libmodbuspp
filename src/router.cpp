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
#include "router_p.h"
#include "config.h"

using json = nlohmann::json;

namespace Modbus {

  // ---------------------------------------------------------------------------
  //
  //                         Router Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Router::Router (Router::Private &dd) : Server (dd) {}

  // ---------------------------------------------------------------------------
  Router::Router () : Server (*new Private (this)) {}

  // ---------------------------------------------------------------------------
  Router::Router (Net net, const std::string & connection,
                  const std::string & settings) : Router () {
    PIMP_D (Router);

    d->setBackend (net, connection, settings);
  }

  // ---------------------------------------------------------------------------
  Router::Router (const std::string & jsonfile,
                  const std::string & key) : Router () {
    PIMP_D (Router);

    d->setConfigFromFile (jsonfile, key);
  }

  // ---------------------------------------------------------------------------
  Router::~Router() {}

  // ---------------------------------------------------------------------------
  void Router::close() {

    if (isOpen()) {
      PIMP_D (Router);

      Server::close();
      for (auto m : d->master) {

        m.second->close();
      }
    }
  }

  // ---------------------------------------------------------------------------
  bool Router::hasMaster (const std::string & name) const {
    PIMP_D (const Router);

    return d->master.count (name) > 0;
  }

  // ---------------------------------------------------------------------------
  Master & Router::addMaster (const std::string & name) {
    PIMP_D (Router);
    
    if (isOpen()) {

      throw std::logic_error ("Unable to add master when open !");
    }
    return *d->addMaster (name);
  }

  // ---------------------------------------------------------------------------
  Master & Router::addMaster (const std::string & name,
                              Net net, const std::string & connection,
                              const std::string & settings) {

    Master & m = addMaster (name);
    m.setBackend (net, connection, settings);
    return m;
  }

  // ---------------------------------------------------------------------------
  Master & Router::master (const std::string & name) {
    PIMP_D (Router);

    return *d->master.at (name);
  }

  // ---------------------------------------------------------------------------
  const Master & Router::master (const std::string & name) const {
    PIMP_D (const Router);

    return *d->master.at (name);
  }

  // ---------------------------------------------------------------------------
  Master * Router::masterPtr (const std::string & name) {
    PIMP_D (Router);

    return hasMaster (name) ? d->master.at (name).get() : nullptr;
  }

  // ---------------------------------------------------------------------------
  const Master * Router::masterPtr (const std::string & name) const {
    PIMP_D (const Router);

    return hasMaster (name) ? d->master.at (name).get() : nullptr;
  }

  // ---------------------------------------------------------------------------
  Master & Router::operator[] (const std::string & name) {
    PIMP_D (Router);

    return * d->master.at (name);
  }

  // ---------------------------------------------------------------------------
  const Master & Router::operator[] (const std::string & name) const {
    PIMP_D (const Router);

    return * d->master.at (name);
  }

  // ---------------------------------------------------------------------------
  const std::map <std::string, std::shared_ptr<Master>> & Router::masters() const {
    PIMP_D (const Router);

    return d->master;
  }

  // ---------------------------------------------------------------------------
  //
  //                         Router::Private Class
  //
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  Router::Private::Private (Router * q) : Server::Private (q) {}

  // ---------------------------------------------------------------------------
  Router::Private::~Private() = default;

  // ---------------------------------------------------------------------------
  // virtual
  void Router::Private::setConfig (const nlohmann::json & config) {
    PIMP_Q (Router);

    Json::setConfig (q, config);
  }

  // ---------------------------------------------------------------------------
  // virtual
  bool Router::Private::open() {

    bool isOk = true;
    for (auto m : master) {

      isOk = isOk && m.second->open();
    }

    if (isOk) {

      isOk = Server::Private::open();
    }

    if (!isOk) {

      for (auto m : master) {

        m.second->close();
      }
    }
    return isOk;
  }

  // ---------------------------------------------------------------------------
  Master * Router::Private::addMaster (const std::string & name) {

    if (master.count (name)) {

      master.erase (name);
    }

    std::shared_ptr<Master> m =
      std::make_shared<Master> ();
    master[name] = m;
    return m.get();
  }

  // ---------------------------------------------------------------------------
  //
  //                         Modbus::Json Namespace
  //
  // ---------------------------------------------------------------------------
  namespace Json {

    // -------------------------------------------------------------------------
    void setConfig (Router * router, const nlohmann::json & j) {

      setConfig (reinterpret_cast<Device *> (router), j);
      if (j.contains ("masters")) {
        auto masters = j["masters"];

        for (const auto & mconfig : masters) {
          auto name = mconfig["name"].get<std::string>();

          Master * mb = & router->addMaster (name);
          setConfig (reinterpret_cast<Device *> (mb), mconfig);

          if (mconfig.contains ("slaves")) {
            auto slaves = mconfig["slaves"];

            for (const auto & sconfig : slaves) {

              auto id = sconfig["id"].get<int>();
              BufferedSlave * slv = & router->addSlave (id, mb);
              setConfig (slv, sconfig);
            }
          }
        }
      }
    }
  }
}
/* ========================================================================== */
