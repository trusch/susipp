/*
 * Copyright (c) 2014, webvariants GmbH, http://www.webvariants.de
 *
 * This file is released under the terms of the MIT license. You can find the
 * complete text in the attached LICENSE file or online at:
 *
 * http://www.opensource.org/licenses/mit-license.php
 *
 * @author: Tino Rusch (tino.rusch@webvariants.de)
 */

#include "webstack/HttpServer.h"
#include "events/EventSystem.h"
#include "sessions/SessionManager.h"
#include "db/Database.h"
#include "db/DBEventInterface.h"
#include "pluginloader/PluginLoader.h"
#include "apiserver/TCPServer.h"
#include "sessions/SessionManagerEventInterface.h"

#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Util/ServerApplication.h>
#include <Poco/Dynamic/Struct.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/AbstractConfiguration.h>
#include <thread>
#include <vector>

using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::AbstractConfiguration;
using Poco::Util::OptionCallback;

class SusiApp : public Poco::Util::ServerApplication {
protected:

	bool _helpRequested = false;

	void defineOptions(OptionSet& options)
	{
		Application::defineOptions(options);

		options.addOption(
			Option("help", "h", "display help information on command line arguments")
				.required(false)
				.repeatable(false)
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handleHelp)));

		options.addOption(
			Option("define", "D", "define a configuration property")
				.required(false)
				.repeatable(true)
				.argument("name=value")
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handleDefine)));

		options.addOption(
			Option("config-file", "f", "load configuration data from a file")
				.required(false)
				.repeatable(true)
				.argument("file")
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handleConfig)));

		options.addOption(
			Option("print", "p", "print configuration data ")
				.required(false)
				.repeatable(false)
				.callback(OptionCallback<SusiApp>(this, &SusiApp::handlePrint)));
	}

	void handleHelp(const std::string& name, const std::string& value){
		_helpRequested = true;
		displayHelp();
	}

	void handleDefine(const std::string& name, const std::string& value){
		defineProperty(value);
	}

	void handleConfig(const std::string& name, const std::string& value){
		std::cout<<"load config "<<value<<std::endl;
		loadConfiguration(value);
	}

	void handlePrint(const std::string& name, const std::string& value){
		setupDefaultProperties();
		printProperties("");
	}

	void displayHelp(){
		HelpFormatter helpFormatter(options());
		helpFormatter.setCommand(commandName());
		helpFormatter.setUsage("OPTIONS");
		helpFormatter.setHeader("SUSI - a Universal System Interface");
		helpFormatter.format(std::cout);
	}

	void defineProperty(const std::string& def){
		std::string name;
		std::string value;
		std::string::size_type pos = def.find('=');
		if (pos != std::string::npos)
		{
			name.assign(def, 0, pos);
			value.assign(def, pos + 1, def.length() - pos);
		}
		else name = def;
		config().setString(name, value);
	}

	void printProperties(const std::string& base)
	{
		AbstractConfiguration::Keys keys;
		config().keys(base, keys);
		if (keys.empty())
		{
			if (config().hasProperty(base))
			{
				std::string msg;
				msg.append(base);
				msg.append(" = ");
				msg.append(config().getString(base));
				logger().information(msg);
			}
		}
		else
		{
			for (AbstractConfiguration::Keys::const_iterator it = keys.begin(); it != keys.end(); ++it)
			{
				std::string fullKey = base;
				if (!fullKey.empty()) fullKey += '.';
				fullKey.append(*it);
				printProperties(fullKey);
			}
		}
	}

	void setupDefaultProperties(){
		auto & cfg = config();
		cfg.setInt("session.lifetime",cfg.getInt("session.lifetime",20));
		cfg.setInt("webstack.port",cfg.getInt("webstack.port",8080));
		cfg.setString("tcpserver.addr",cfg.getString("tcpserver.addr","localhost:4000"));
		cfg.setString("webstack.assets",cfg.getString("webstack.assets","./assets/"));
		cfg.setString("jsengine.source",cfg.getString("jsengine.source","./controller.js"));
	}

	int main(const std::vector<std::string> &){
		if(!_helpRequested){

			Susi::subscribe("*",[](Susi::Event & event){
				std::cout<<"Event: "<<event.toString()<<std::endl;
			});

			setupDefaultProperties();

			int sessionLifetime = config().getInt("session.lifetime");
			unsigned short httpPort = config().getInt("webstack.port");
			std::string tcpAddr = config().getString("tcpserver.addr");
			std::string jsSource = config().getString("jsengine.source");

			Susi::initDBEventInterface();
			Susi::sessionManager.init(std::chrono::seconds(sessionLifetime),std::chrono::seconds(5));
			Susi::initSessionManagerEventInterface();
			//testDB();


			auto httpParams = new Poco::Net::HTTPServerParams();
			httpParams->setMaxQueued(250);
			Susi::HttpServer httpServer(httpPort,httpParams);

			auto tcpParams = new Poco::Net::TCPServerParams();
			tcpParams->setMaxQueued(250);
			Susi::TCPServer tcpServer(tcpAddr,tcpParams);	

			std::cout << "Server started" << std::endl;

			waitForTerminationRequest();  // wait for CTRL-C or kill
			std::cout << std::endl << "Shutting down..." << std::endl;
			httpServer.stop();
			tcpServer.stop();
		}
		return Application::EXIT_OK;
	}

	void testDB(){
		std::string query = "SELECT main.ALL_NR as id, \
                    big.AL_TEXT as text, \
                    cast(main.ALL_EVENTTIME as VARCHAR(64)) as stamp, \
                    sev.ALS_TEXT as severity, \
                    device.UT_SYSNAME as sysname, \
                    coalesce(cast(device.UT_LONGITUDE as VARCHAR(32)),'') as longitude, \
                    coalesce(cast(device.UT_LATITUDE as VARCHAR(32)),'') as latitude, \
                    alarmclasstable.EVT_TEXT as alarmclass, \
                    cause.ALP_TEXT as cause \
                FROM TALL_ALARMLIST as main \
                LEFT JOIN TAL_ALARM as big ON main.ALL_AL_NR = big.AL_NR \
                LEFT JOIN TALS_ALARMSEVERITY AS sev ON big.AL_ALS_NR_DEF = sev.ALS_NR \
                LEFT JOIN TUT_UNIT AS device ON main.ALL_UT_NR = device.UT_NR \
                LEFT JOIN TEVT_EVENTTYPE AS alarmclasstable ON big.AL_EVT_NR = alarmclasstable.EVT_NR \
                LEFT JOIN TALP_ALARMPROBABLECAUSE AS cause ON big.AL_ALP_NR = cause.ALP_NR;";
		auto payload = Susi::Event::Payload({
			{"type","firebird"},
			{"uri","service=/usr/share/susi/db/AIMOS.FB user=AIMOS password=AIMOSPW"},
			{"query",query}
		});
		Susi::Event event("db::query",payload,"","result");
		Susi::subscribe("result",[](Susi::Event & event){
			auto alarms = event.payload.extract<std::vector<Poco::Dynamic::Var>>();
			std::cout<<"got "<<alarms.size()<<" alarms!"<<std::endl;
		});
		Susi::publish(event);
	}
};

int main(int argc, char** argv){
	SusiApp app;
	return app.run(argc, argv);
}
