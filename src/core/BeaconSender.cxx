/**
* Copyright 2018 Dynatrace LLC
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "BeaconSender.h"

#include <memory>
#include <chrono>

#include "communication/BeaconSendingInitialState.h"
#include "communication/BeaconSendingContext.h"

using namespace core;
using namespace communication;
using namespace providers;

constexpr int32_t SHUTDOWN_TIMEOUT = 10 * 1000;
constexpr int32_t SHUTDOWN_SLICED_WAIT_TIME = 100;

BeaconSender::BeaconSender(std::shared_ptr<openkit::ILogger> logger,
						   std::shared_ptr<configuration::Configuration> configuration,
						   std::shared_ptr<providers::IHTTPClientProvider> httpClientProvider,
						   std::shared_ptr<providers::ITimingProvider> timingProvider)
	: mLogger(logger)
	, mBeaconSendingContext(std::shared_ptr<BeaconSendingContext>(new BeaconSendingContext(logger, httpClientProvider, timingProvider, configuration)))
	, mSendingThread()
	, mShutdownTrigger(false)
	, mTimingProvider(timingProvider)
{

}

bool BeaconSender::initialize()
{
	mSendingThread = std::async(std::launch::async, [this] {
		// run the loop as long as OpenKit does not get shutdown or ends itself.
		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconSender thread started");
		}

		while (mBeaconSendingContext != nullptr && !mBeaconSendingContext->isInTerminalState() && !mShutdownTrigger)
		{
			mBeaconSendingContext->executeCurrentState();
		}

		if (mLogger->isDebugEnabled())
		{
			mLogger->debug("BeaconSender thread stopped");
		}

		return mBeaconSendingContext->isShutdownRequested();
	});
	
	return true;
}

bool BeaconSender::waitForInit() const
{
	return mBeaconSendingContext->waitForInit();
}

bool BeaconSender::waitForInit(int64_t timeoutMillis) const
{
	return mBeaconSendingContext->waitForInit(timeoutMillis);
}

bool BeaconSender::isInitialized() const
{
	return mBeaconSendingContext->isInitialized();
}

void BeaconSender::shutdown()
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconSender thread request shutdown");
	}

	mBeaconSendingContext->requestShutdown();
	mShutdownTrigger = true;

	auto start = mTimingProvider->provideTimestampInMilliseconds();
	int64_t timePassed = 0;
	while (timePassed < SHUTDOWN_TIMEOUT)
	{
		//sleep in slices of 100ms
		auto threadStatus = mSendingThread.wait_for(std::chrono::milliseconds(SHUTDOWN_SLICED_WAIT_TIME));
		if (threadStatus == std::future_status::ready)
		{
			return;//thread finished before tinout happened
		}
		timePassed = mTimingProvider->provideTimestampInMilliseconds() - start;
	}

	// if the thread is still running get rid of it
	mSendingThread._Abandon();
}

void BeaconSender::startSession(std::shared_ptr<Session> session)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconSender startSession");
	}
	mBeaconSendingContext->startSession(session);
}

void BeaconSender::finishSession(std::shared_ptr<Session> session)
{
	if (mLogger->isDebugEnabled())
	{
		mLogger->debug("BeaconSender finishSession");
	}
	mBeaconSendingContext->finishSession(session);
}
