/* Copyright 2013-2017 Sathya Laufer
 *
 * Homegear is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Homegear is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Homegear.  If not, see <http://www.gnu.org/licenses/>.
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */

#include "MyNode.h"

namespace MyNode
{

MyNode::MyNode(std::string path, std::string nodeNamespace, std::string type, const std::atomic_bool* frontendConnected) : Flows::INode(path, nodeNamespace, type, frontendConnected)
{
	_stopThread = true;
	_enabled = true;
}

MyNode::~MyNode()
{
	_stopThread = true;
	waitForStop();
}


bool MyNode::init(Flows::PNodeInfo info)
{
	try
	{
		auto settingsIterator = info->info->structValue->find("startup");
		if(settingsIterator != info->info->structValue->end()) _outputOnStartUp = settingsIterator->second->booleanValue;

		settingsIterator = info->info->structValue->find("ontime");
		if(settingsIterator != info->info->structValue->end()) _onTime = settingsIterator->second->stringValue;

		settingsIterator = info->info->structValue->find("ontimetype");
		if(settingsIterator != info->info->structValue->end()) _onTimeType = settingsIterator->second->stringValue;

		settingsIterator = info->info->structValue->find("offtime");
		if(settingsIterator != info->info->structValue->end()) _offTime = settingsIterator->second->stringValue;

		settingsIterator = info->info->structValue->find("offtimetype");
		if(settingsIterator != info->info->structValue->end()) _offTimeType = settingsIterator->second->stringValue;

		settingsIterator = info->info->structValue->find("startoff");
		if(settingsIterator != info->info->structValue->end()) _onOffset = Flows::Math::getNumber(settingsIterator->second->stringValue) * 60000;

		settingsIterator = info->info->structValue->find("endoff");
		if(settingsIterator != info->info->structValue->end()) _offOffset = Flows::Math::getNumber(settingsIterator->second->stringValue) * 60000;

		settingsIterator = info->info->structValue->find("lat");
		if(settingsIterator != info->info->structValue->end()) _latitude = Flows::Math::getDouble(settingsIterator->second->stringValue);

		settingsIterator = info->info->structValue->find("lon");
		if(settingsIterator != info->info->structValue->end()) _longitude = Flows::Math::getDouble(settingsIterator->second->stringValue);

		_days.resize(7, true);
		_months.resize(12, true);

		settingsIterator = info->info->structValue->find("sun");
		if(settingsIterator != info->info->structValue->end()) _days.at(0) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("mon");
		if(settingsIterator != info->info->structValue->end()) _days.at(1) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("tue");
		if(settingsIterator != info->info->structValue->end()) _days.at(2) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("wed");
		if(settingsIterator != info->info->structValue->end()) _days.at(3) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("thu");
		if(settingsIterator != info->info->structValue->end()) _days.at(4) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("fri");
		if(settingsIterator != info->info->structValue->end()) _days.at(5) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("sat");
		if(settingsIterator != info->info->structValue->end()) _days.at(6) = settingsIterator->second->booleanValue;

		settingsIterator = info->info->structValue->find("jan");
		if(settingsIterator != info->info->structValue->end()) _months.at(0) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("feb");
		if(settingsIterator != info->info->structValue->end()) _months.at(1) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("mar");
		if(settingsIterator != info->info->structValue->end()) _months.at(2) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("apr");
		if(settingsIterator != info->info->structValue->end()) _months.at(3) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("may");
		if(settingsIterator != info->info->structValue->end()) _months.at(4) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("jun");
		if(settingsIterator != info->info->structValue->end()) _months.at(5) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("jul");
		if(settingsIterator != info->info->structValue->end()) _months.at(6) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("aug");
		if(settingsIterator != info->info->structValue->end()) _months.at(7) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("sep");
		if(settingsIterator != info->info->structValue->end()) _months.at(8) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("oct");
		if(settingsIterator != info->info->structValue->end()) _months.at(9) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("nov");
		if(settingsIterator != info->info->structValue->end()) _months.at(10) = settingsIterator->second->booleanValue;
		settingsIterator = info->info->structValue->find("dec");
		if(settingsIterator != info->info->structValue->end()) _months.at(11) = settingsIterator->second->booleanValue;

		auto enabled = getNodeData("enabled");
		if(enabled->type == Flows::VariableType::tBoolean) _enabled = enabled->booleanValue;
		_lastOnTime = getNodeData("lastOnTime")->integerValue64;
		_lastOffTime = getNodeData("lastOffTime")->integerValue64;

		return true;
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return false;
}

bool MyNode::start()
{
	try
	{
		return true;
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return false;
}

void MyNode::startUpComplete()
{
	try
	{
		std::lock_guard<std::mutex> timerGuard(_timerMutex);
		if(!_enabled) return;
		_stopThread = false;
		if(_timerThread.joinable()) _timerThread.join();
		_timerThread = std::thread(&MyNode::timer, this);
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
}

void MyNode::stop()
{
	try
	{
		_stopThread = true;
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
}

void MyNode::waitForStop()
{
	try
	{
		std::lock_guard<std::mutex> timerGuard(_timerMutex);
		if(_timerThread.joinable()) _timerThread.join();
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
}

std::vector<std::string> MyNode::splitAll(std::string string, char delimiter)
{
	std::vector<std::string> elements;
	std::stringstream stringStream(string);
	std::string element;
	while (std::getline(stringStream, element, delimiter))
	{
		elements.push_back(element);
	}
	if(string.back() == delimiter) elements.push_back(std::string());
	return elements;
}

int64_t MyNode::getSunTime(int64_t timeStamp, std::string time)
{
	try
	{
		auto sunTimes = _sunTime.getTimesLocal(timeStamp, _latitude, _longitude);
		if(time == "sunrise") return sunTimes.times.at(SunTime::SunTimeTypes::sunrise);
		else if(time == "sunset") return sunTimes.times.at(SunTime::SunTimeTypes::sunset);
		else if(time == "sunriseEnd") return sunTimes.times.at(SunTime::SunTimeTypes::sunriseEnd);
		else if(time == "sunsetStart") return sunTimes.times.at(SunTime::SunTimeTypes::sunsetStart);
		else if(time == "dawn") return sunTimes.times.at(SunTime::SunTimeTypes::dawn);
		else if(time == "dusk") return sunTimes.times.at(SunTime::SunTimeTypes::dusk);
		else if(time == "nauticalDawn") return sunTimes.times.at(SunTime::SunTimeTypes::nauticalDawn);
		else if(time == "nauticalDusk") return sunTimes.times.at(SunTime::SunTimeTypes::nauticalDusk);
		else if(time == "nightEnd") return sunTimes.times.at(SunTime::SunTimeTypes::nightEnd);
		else if(time == "night") return sunTimes.times.at(SunTime::SunTimeTypes::night);
		else if(time == "goldenHourEnd") return sunTimes.times.at(SunTime::SunTimeTypes::goldenHourEnd);
		else if(time == "goldenHour") return sunTimes.times.at(SunTime::SunTimeTypes::goldenHour);
		else if(time == "solarNoon") return sunTimes.solarNoon;
		else if(time == "nadir") return sunTimes.nadir;
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return -1;
}

int64_t MyNode::getTime(int64_t currentTime, std::string time, std::string timeType, int64_t offset)
{
	try
	{
		if(timeType == "suntime")
		{
			int32_t i = 0;
			int64_t sunTime = 1;
			int64_t inputTime = currentTime - 86400000;
			while(sunTime + offset < currentTime && sunTime >= 0 && i < 1000)
			{
				sunTime = getSunTime(inputTime, time);
				inputTime += 86400000;
				i++;
			}
			return sunTime + offset;
		}
		else
		{
			auto timeVector = splitAll(time, ':');
			int64_t time = (_sunTime.getLocalTime() / 86400000) * 86400000 + offset - 86400000;
			if(timeVector.size() > 0)
			{
				time += Flows::Math::getNumber64(timeVector.at(0)) * 3600000;
				if(timeVector.size() > 1)
				{
					time += Flows::Math::getNumber64(timeVector.at(1)) * 60000;
					if(timeVector.size() > 2) time += Flows::Math::getNumber64(timeVector.at(2)) * 1000;
				}
			}
			while(time < currentTime) time += 86400000;
			return time;
		}
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return 0;
}

std::pair<int64_t, bool> MyNode::getNext(int64_t currentTime, int64_t onTime, int64_t offTime)
{
	try
	{
		std::pair<int64_t, bool> result = std::make_pair(-1, false);

		if((currentTime >= onTime && currentTime >= offTime)) return result;

		if(onTime >= currentTime && offTime >= currentTime)
		{
			if(onTime > offTime)
			{
				result.first = offTime;
				result.second = false;
			}
			else
			{
				result.first = onTime;
				result.second = true;
			}
		}
		else if(onTime >= currentTime)
		{
			result.first = onTime;
			result.second = true;
		}
		else //offTime >= currentTime
		{
			result.first = offTime;
			result.second = false;
		}

		return result;
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
	return std::make_pair(-1, false);
}

std::string MyNode::getDateString(int64_t time)
{
	const char timeFormat[] = "%x";
	std::time_t t;
	if(time > 0)
	{
		t = std::time_t(time / 1000);
	}
	else
	{
		const auto timePoint = std::chrono::system_clock::now();
		t = std::chrono::system_clock::to_time_t(timePoint);
	}
	char timeString[50];
	std::tm localTime;
	localtime_r(&t, &localTime);
	strftime(&timeString[0], 50, &timeFormat[0], &localTime);
	std::ostringstream timeStream;
	timeStream << timeString;
	return timeStream.str();
}

void MyNode::printNext(int64_t currentTime, int64_t onTime, int64_t offTime)
{
	try
	{
		auto next = getNext(currentTime, onTime, offTime);

		Flows::PVariable status = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
		if(next.first == -1)
		{
			status->structValue->emplace("text", std::make_shared<Flows::Variable>("Next: Unknown"));
			nodeEvent("statusBottom/" + _id, status);
			return;
		}

		std::ostringstream timeStream;
		if(next.first > currentTime + 86400000)
		{
			timeStream << getDateString(next.first);
		}
		else
		{
			next.first = next.first / 1000;
			next.first = next.first % 86400;
			int32_t hours = next.first / 3600;
			next.first = next.first % 3600;
			int32_t minutes = next.first / 60;
			int32_t seconds = next.first % 60;
			timeStream << std::setw(2) << std::setfill('0') << hours << ':' << std::setw(2) << minutes << ':' << std::setw(2) << seconds;
		}

		status->structValue->emplace("text", std::make_shared<Flows::Variable>("Next: " + timeStream.str() + " (" + (next.second ? "on" : "off") + ")"));
		nodeEvent("statusBottom/" + _id, status);
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
}

void MyNode::timer()
{
	bool event = false;
	bool update = false;
	int64_t currentTime = _sunTime.getLocalTime();
	int64_t onTime = getTime(currentTime, _onTime, _onTimeType, _onOffset);
	int64_t offTime = getTime(currentTime, _offTime, _offTimeType, _offOffset);
	int32_t day = 0;
	int32_t month = 0;

	{
		std::tm tm;
		_sunTime.getTimeStruct(tm);
		day = tm.tm_wday;
		month = tm.tm_mon;
	}

	printNext(currentTime, onTime, offTime);

	if(_outputOnStartUp)
	{
		auto next = getNext(currentTime, onTime, offTime);
		if(next.first != -1)
		{
			Flows::PVariable message = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
			message->structValue->emplace("payload", std::make_shared<Flows::Variable>(!next.second));
			output(0, message);
		}
	}

	while(!_stopThread)
	{
		try
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			if(_stopThread) break;
			Flows::PVariable message = std::make_shared<Flows::Variable>(Flows::VariableType::tStruct);
			currentTime = _sunTime.getLocalTime();
			if(_lastOnTime < onTime && currentTime >= onTime && _lastOffTime < offTime && currentTime >= offTime)
			{
				if(onTime > offTime) _lastOffTime = offTime;
				else _lastOnTime = onTime;
			}
			if(_lastOnTime < onTime && currentTime >= onTime)
			{
				update = true;
				if(_days.at(day) && _months.at(month))
				{
					event = true;
					_lastOnTime = onTime;
					setNodeData("lastOnTime", std::make_shared<Flows::Variable>(_lastOnTime));
					message->structValue->emplace("payload", std::make_shared<Flows::Variable>(true));
					if(onTime == offTime) _lastOffTime = offTime;
				}
			}
			if(_lastOffTime < offTime && currentTime >= offTime)
			{
				update = true;
				if(_days.at(day) && _months.at(month))
				{
					event = true;
					_lastOffTime = offTime;
					setNodeData("lastOffTime", std::make_shared<Flows::Variable>(_lastOffTime));
					message->structValue->emplace("payload", std::make_shared<Flows::Variable>(false));
				}
			}
			if(event)
			{
				event = false;
				output(0, message);
			}
			if(update)
			{
				update = false;
				onTime = getTime(currentTime, _onTime, _onTimeType, _onOffset);
				offTime = getTime(currentTime, _offTime, _offTimeType, _offOffset);
				{
					std::tm tm;
					_sunTime.getTimeStruct(tm);
					day = tm.tm_wday;
					month = tm.tm_mon;
				}
				printNext(currentTime, onTime, offTime);
			}
		}
		catch(const std::exception& ex)
		{
			Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
		}
		catch(...)
		{
			Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
		}
	}
}

void MyNode::input(Flows::PNodeInfo info, uint32_t index, Flows::PVariable message)
{
	try
	{
		_enabled = message->structValue->at("payload")->booleanValue;
		setNodeData("enabled", std::make_shared<Flows::Variable>(_enabled));
		std::lock_guard<std::mutex> timerGuard(_timerMutex);
		if(_enabled)
		{
			if(!_stopThread)
			{
				_stopThread = true;
				if(_timerThread.joinable()) _timerThread.join();
				_stopThread = false;
				_timerThread = std::thread(&MyNode::timer, this);
			}
		}
		else
		{
			_stopThread = true;
			if(_timerThread.joinable()) _timerThread.join();
		}
	}
	catch(const std::exception& ex)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__, ex.what());
	}
	catch(...)
	{
		Flows::Output::printEx(__FILE__, __LINE__, __PRETTY_FUNCTION__);
	}
}

}
