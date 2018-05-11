#include <sstream>

    #include <fstream>

    #include <iostream>

    #include <cassert>

    #include <cmath>

    #include <stdexcept>



    #include "geometry.h"

    #include "xmlparser.h"

    #include "track.h"



    using namespace GPS;



    // Note: The implementation should exploit the relationship:

    //   totalTime() == restingTime() + travellingTime()



    seconds Track::totalTime() const

    {

        const bool implemented = false;

        assert(implemented);

    }



    seconds Track::restingTime() const

    {

        const bool implemented = false;

        assert(implemented);

    }



    seconds Track::travellingTime() const

    {

        const bool implemented = false;

        assert(implemented);

    }



    speed Track::maxSpeed() const

    {

        const bool implemented = false;

        assert(implemented);

    }



    speed Track::averageSpeed(bool includeRests) const

    {

        const bool implemented = false;

        assert(implemented);

    }


    //N0687265
    speed Track::maxRateOfAscent() const
    {
        const bool implemented = true;
        assert(implemented);

        seconds time;
        metres elevation;
        speed rate, topRate = 0;
        for(unsigned long int i = 1; i < positions.size(); ++i){
            time = arrived[i] - departed[i-1];
            elevation = positions[i].elevation() - positions[i-1].elevation();
            rate = elevation / time;
            if(rate > topRate){
                topRate = rate;
            }
        }

        return topRate;
    }




    speed Track::maxRateOfDescent() const

    {

        const bool implemented = false;

        assert(implemented);

    }



    Track::Track(std::string sourceFile, bool isFileName, metres granularity)

    {

        using namespace std;

        using namespace XML::Parser;

        string mergedTrkSegs,trkseg,lat,lon,ele,name,time,temp,temp2;

        //metres deltaH,deltaV;

        seconds startTime, currentTime, timeElapsed;

        ostringstream reportInfo,fileOutput;

        string InFile = "";

        this->granularity = granularity;

        if (isFileName) {

            ifstream fs(sourceFile);

            if (! fs.good()) throw invalid_argument("Error opening source file '" + sourceFile + "'.");

            reportInfo << "Source file '" << sourceFile << "' opened okay." << endl;

            string line = "";

            while (fs.good()) {

                getline(fs, line);

                fileOutput << line << endl;

            }

            InFile = fileOutput.str();

        }

        if (! elementExists(InFile,"gpx")) throw domain_error("No 'gpx' element.");

        //temp = getElement(source, "gpx");

        string GPXData = getElementContent(getElement(InFile, "gpx"));

        if (! elementExists(GPXData,"trk")) throw domain_error("No 'trk' element.");

        //temp = getElement(source, "trk");

        string trackData = getElementContent(getElement(GPXData, "trk"));

        if (elementExists(trackData, "name")) {

            //temp = getAndEraseElement(source, "name");

            routeName = getElementContent(getAndEraseElement(trackData, "name"));

            reportInfo << "Track name is: " << routeName << endl;

        }

        string source = trackData;

        while (elementExists(source, "trkseg")) {

            //temp = getAndEraseElement(source, "trkseg");

            trkseg = getElementContent(getAndEraseElement(source, "trkseg"));

            getAndEraseElement(trkseg, "name");

            mergedTrkSegs += trkseg;

        }

        if (! mergedTrkSegs.empty()) source = mergedTrkSegs;

        unsigned int numOfPositions = 0;

        if (! elementExists(source,"trkpt")) throw domain_error("No 'trkpt' element.");

        string trackPosition = getAndEraseElement(source, "trkpt");



        /**/

        Position startPos = getPosition(trackPosition);

        positions.push_back(startPos);

        reportInfo << "Start position added: " << startPos.toString() << endl;

        ++numOfPositions;



        /**/
        positionNames.push_back(addName(trackPosition));



        arrived.push_back(0);

        departed.push_back(0);



        startTime = currentTime = getTime(trackPosition);

        Position prevPos = positions.back(), nextPos = positions.back();

        while (elementExists(source, "trkpt")) {

            string trackPosition = getAndEraseElement(source, "trkpt");

            /**/

            nextPos = getPosition(trackPosition);


            currentTime = getTime(trackPosition);//stringToTime(time);

            if (areSameLocation(nextPos, prevPos)) {

                // If we're still at the same location, then we haven't departed yet.

                departed.back() = currentTime - startTime;

                reportInfo << "Position ignored: " << nextPos.toString() << endl;

            } else {



                positions.push_back(nextPos);

                positionNames.push_back(addName(trackPosition));

                timeElapsed = currentTime - startTime;

                arrived.push_back(timeElapsed);

                departed.push_back(timeElapsed);

                reportInfo << "Position added: " << nextPos.toString() << endl;

                reportInfo << " at time: " << to_string(timeElapsed) << endl;

                ++numOfPositions;

                prevPos = nextPos;

            }

        }

        reportInfo << numOfPositions << " positions added." << endl;

        routeLength = 0;

        metres flatDistance, elevationBetweenPoints;

        for (unsigned int i = 1; i < numOfPositions; ++i ) {

            flatDistance = distanceBetween(positions[i-1], positions[i]);

            elevationBetweenPoints = positions[i-1].elevation() - positions[i].elevation();

            routeLength += sqrt(pow(flatDistance,2) + pow(elevationBetweenPoints,2));

        }

        report = reportInfo.str();

    }

    Position Track::getPosition(std::string source)
    {


        if (! XML::Parser::attributeExists(source,"lat")) throw std::domain_error("No 'lat' attribute.");

        if (! XML::Parser::attributeExists(source,"lon")) throw std::domain_error("No 'lon' attribute.");

        std::string lat = XML::Parser::getElementAttribute(source, "lat");

        std::string lon = XML::Parser::getElementAttribute(source, "lon");

        std::string elevationSource = XML::Parser::getElementContent(source);

            if (XML::Parser::elementExists(elevationSource, "ele")) {

                //temp2 = getElement(temp, "ele");

                std::string ele = XML::Parser::getElementContent(XML::Parser::getElement(elevationSource, "ele"));

                Position foundPosition = Position(lat,lon,ele);

                return foundPosition;

            } else
            {
                Position foundPosition = Position(lat,lon);

                return foundPosition;
            }

    }

    seconds Track::getTime(std::string source)
    {
        std::string content = "";

        content = XML::Parser::getElementContent(source);

        if (! XML::Parser::elementExists(content,"time")) throw std::domain_error("No 'time' element.");

        std::string time = XML::Parser::getElementContent(XML::Parser::getElement(content,"time"));

        return stringToTime(time);

    }

    std::string Track::addName(std::string source)
    {
        std::string name = "";

        std::string content = XML::Parser::getElementContent(source);

        //this code is repeated
        if (XML::Parser::elementExists(content,"name")) {



            name = XML::Parser::getElementContent(XML::Parser::getElement(content,"name"));

        }

        return name;

    }

    void Track::setGranularity(metres granularity)

    {

        bool implemented = false;

        assert(implemented);

    }



    seconds Track::stringToTime(const std::string & timeStr)

    {

        return stoull(timeStr);

    }

