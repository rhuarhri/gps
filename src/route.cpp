#include <sstream>
#include <fstream>
#include <iostream>
#include <cassert>
#include <cmath>
#include <stdexcept>

#include "xmlparser.h"
#include "geometry.h"
#include "earth.h"
#include "route.h"
#include "position.h"
using namespace GPS;

std::string Route::name() const
{
   return routeName.empty() ? "Unnamed Route" : routeName;
}

unsigned int Route::numPositions() const
{
    const bool implemented = true;
    assert(implemented);
    
    return positions.size();
}

metres Route::totalLength() const
{
    return routeLength;
}

metres Route::netLength() const
{
    const bool implemented = true;
    assert(implemented);



    metres distance = distanceBetween(positions[0],positions.back());
    return distance;
}

metres Route::totalHeightGain() const
{
    metres heightGain = 0;
    if (positions.size() > 1)
    {
        for (unsigned int i = 1; i<positions.size(); ++i)
        {
            if (positions[i].elevation()-positions[i-1].elevation()>0)
            {
                heightGain += positions[i].elevation()-positions[i-1].elevation();
            }
        }
    }
    return heightGain;
}

metres Route::netHeightGain() const
{
    std::vector<Position>::const_iterator first, last;
    first = positions.begin();
    last =  positions.end() - 1;
    if (last->elevation() - first->elevation() > 0){
        return last->elevation() - first->elevation();
    }
    else{
        return 0;
    }
}

degrees Route::minLatitude() const
{
    const bool implemented = true;
    assert(implemented);
//    degrees minLat = positions[0].latitude();
//    for(unsigned int i = 1; i < positions.length(); i++)
//    {
//        if(positions[i].latitude() < minLat)
//        {
//            minLat = positions[i].latitude();
//        }
//    }
//    return minLat;
    degrees minLat = positions[0].latitude();
    for(unsigned int i = 1; i < positions.size(); i++)
    {
        if(positions[i].latitude() < minLat)
        {
            minLat = positions[i].latitude();
        }
    }
    if (minLat < -90 || minLat > 90)
        throw std::out_of_range ("out of range");
    return minLat;
}

degrees Route::maxLatitude() const
{
    const bool implemented = true;

    assert(implemented);

    int Maximum = 0;
    for (int i = 0; i < positions.size() ; i++)
    {

        if (positions[i].latitude() > positions[Maximum].latitude()) {
            Maximum = i;
        }

    }

    return positions[Maximum].latitude();
}

degrees Route::minLongitude() const
{
     const bool implemented = false;
    assert(implemented);

}

degrees Route::maxLongitude() const
{
    const bool implemented = true;

    assert(implemented);

    int MaximumIndex = 0;
    for (unsigned int i = 0; i < positions.size() ; i++)
    {

        if ((positions[i].longitude() > 180) || (positions[i].longitude() < -180)){
            throw std::out_of_range("Not in bounds of long");
        }

        if (positions[i].longitude() > positions[MaximumIndex].longitude()) {
            MaximumIndex = i;
        }

    }

    return positions[MaximumIndex].longitude();
}

metres Route::minElevation() const
{
    degrees minElev = positions[0].elevation();
    for(auto pos : positions){
        if(pos.elevation() < minElev)
            minElev = pos.elevation();
    }
    return minElev;
}

metres Route::maxElevation() const // N0669298
{
    if (positions.size() == 0)
    {
        throw std::invalid_argument("No positions in provided route");
    }    
    degrees maxElev = positions[0].elevation();
    for(auto pos : positions){
        if(pos.elevation() > maxElev)
            maxElev = pos.elevation();
    }
    return maxElev;
}

degrees Route::maxGradient() const
{
    const bool implemented = true;
    assert(implemented);
    //
    if (positions.size()<2) return 0;


    degrees largestGradient;
    degrees testGradient;
 
    if (distanceBetween(positions[0],positions[1])>0) {
        largestGradient =radToDeg(atan2((positions[1].elevation() - positions[0].elevation()),distanceBetween(positions[1],positions[0])));
        testGradient = largestGradient;

    }

    else {
        if (positions[0].elevation() <positions[1].elevation()){
        largestGradient =90;
        testGradient = 90;}
        else if (positions[0].elevation() >positions[1].elevation())
        {
            largestGradient =-90;
            testGradient = -90;
        }
        else{
            largestGradient =0;
            testGradient = 0;
        }
}
    for(size_t x = 2; x < positions.size(); x++){

        if (distanceBetween(positions[x],positions[x-1])>0){
          testGradient =
                  radToDeg(atan2((positions[x].elevation() - positions[x-1].elevation()),
                  distanceBetween(positions[x],positions[x-1])));
        }
        else if (positions[0].elevation() <positions[1].elevation())
        {
            testGradient = 90;
        }
        else if (positions[0].elevation() >positions[1].elevation())
        {
            testGradient = -90;
        }
        else
        {
            testGradient = 0;
        }


        if(testGradient > largestGradient){
             largestGradient = testGradient;
        }
        
     }
    return largestGradient;
}

degrees Route::minGradient() const
{
    const bool implemented = true;
    assert(implemented);

    if (positions.size()<2)
    {
        throw std::invalid_argument("Gradient cannot be worked out when using only one point");
    }

    degrees minGradient;
    degrees temp, deltaH,deltaV;

    deltaH = distanceBetween(positions[0], positions[1]);
    deltaV = positions[0].elevation() - positions[1].elevation();

    minGradient=sqrt(pow(deltaH,2) + pow(deltaV,2));
    if(deltaV==0)
    {
        minGradient=0;
    }
    if (positions[0].elevation() > positions[1].elevation())
    {
        minGradient = -minGradient;
    }
    for (unsigned int i = 2; i < positions.size(); ++i )
    {
        deltaH = distanceBetween(positions[i-1], positions[i]);
        deltaV = positions[i-1].elevation() - positions[i].elevation();
        temp= sqrt(pow(deltaH,2) + pow(deltaV,2));
        if(deltaV==0)
        {
            temp=0;
        }
        if(positions[i-1].elevation() > positions[i].elevation())
        {
            temp = -temp;
        }

        if(temp<minGradient)
        {
            minGradient=temp;
        }
    }
    return minGradient;

}

degrees Route::steepestGradient() const
{
    const bool implemented = false;
    assert(implemented);
}

Position Route::operator[](unsigned int idx) const
{
    return positions.at(idx);
}

Position Route::findPosition(const std::string & soughtName) const
{
    const bool implemented = false;
    assert(implemented);
}

std::string Route::findNameOf(const Position & soughtPos) const
{
    const bool implemented = false;
    assert(implemented);
}

unsigned int Route::timesVisited(const std::string & soughtName) const
{
    const bool implemented = false;
    assert(implemented);
}

unsigned int Route::timesVisited(const Position & soughtPos) const
{
    const bool implemented = false;
    assert(implemented);
}

std::string Route::buildReport() const
{
    return report;
}

Route::Route(std::string sourceFile, bool isFileName, metres granularity)
{
    //should have 56 errors

    using namespace std; //what is the std class used for?
    using namespace XML::Parser;


    ostringstream reportInfo,fileOutput; /*these variables were changed from oss and oss2 because
                                            these name do not discribe what they do*/
    unsigned int posistionsAmount = 0;
    this->granularity = granularity;
    string InFile  = "";
    if (isFileName){
        ifstream fs(sourceFile);
        if (! fs.good()) throw invalid_argument("Error opening source file '" + sourceFile + "'.");
        reportInfo << "Source file '" << sourceFile << "' opened okay." << endl;
        string line = "";//why here because it is only used hear
        while (fs.good()) {
            getline(fs, line);
            fileOutput << line << endl;
        }
        InFile = fileOutput.str();
    }
    if (! elementExists(InFile,"gpx")) throw domain_error("No 'gpx' element.");
    /*temp = getElement(source, "gpx");
    source = getElementContent(temp); was changed to the code below as both do the smae thing*/
    string InGPX = getElementContent(getElement(InFile, "gpx"));
    if (! elementExists(InGPX,"rte")) throw domain_error("No 'rte' element.");

    string InRTE = getElementContent(getElement(InGPX, "rte"));
    if (elementExists(InRTE, "name")) {
        routeName = getElementContent(getAndEraseElement(InRTE, "name"));
        reportInfo << "Route name is: " << routeName << endl;
    }


    //string lat,lon,ele,name;//these variables were moved as they are only used from here on
    if (! elementExists(InRTE,"rtept")) throw domain_error("No 'rtept' element.");
    string routePoint = getAndEraseElement(InRTE, "rtept");

    Position startPos = getPosition(routePoint);


    positions.push_back(startPos);
    reportInfo << "Position added: " << startPos.toString() << endl;
    ++posistionsAmount;

    addName(routePoint);

    Position prevPos = positions.back(), nextPos = positions.back();

    string fileData = "";
    while (elementExists(InRTE, "rtept")) {

        fileData = getAndEraseElement(InRTE, "rtept");

        /*

        if (! attributeExists(fileData,"lat")) throw domain_error("No 'lat' attribute.");
        if (! attributeExists(fileData,"lon")) throw domain_error("No 'lon' attribute.");
        lat = getElementAttribute(fileData, "lat");
        lon = getElementAttribute(fileData, "lon");
        string fileContent = "";
        fileContent = getElementContent(fileData);
        if (elementExists(fileContent, "ele")) {
            /*temp2 = getElement(temp, "ele"); //would it be better to have just ele = getElement(temp,"ele");
            ele = getElementContent(temp2);*
            ele = getElementContent(getElement(fileContent, "ele"));
            nextPos = Position(lat,lon,ele);
        } else nextPos = Position(lat,lon);*/

        nextPos = getPosition(fileData);

        if (areSameLocation(nextPos, prevPos)) reportInfo << "Position ignored: " << nextPos.toString() << endl;
        else {
            /*
            if (elementExists(fileContent,"name")) {
                /*temp2 = getElement(temp,"name");
                name = getElementContent(temp2);* //would it be better to have just name = getElement(temp,"name");
                name = getElementContent(getElement(fileContent,"name"));
            } else name = ""; // Fixed bug by adding this.*/
            positions.push_back(nextPos);
            positionNames.push_back(addName(routePoint));
            reportInfo << "Position added: " << nextPos.toString() << endl;
            ++posistionsAmount;
            prevPos = nextPos;
        }
    }
    reportInfo << posistionsAmount << " positions added." << endl;

    routeLength = 0;
    metres flatDistance,elevationBetweenPoints;
    for (unsigned int i = 1; i < posistionsAmount; ++i ) {
        flatDistance = distanceBetween(positions[i-1], positions[i]);
        elevationBetweenPoints = positions[i-1].elevation() - positions[i].elevation();
        routeLength += sqrt(pow(flatDistance,2) + pow(elevationBetweenPoints,2));
    }
    report = reportInfo.str();
}

std::string Route::addName(std::string source)
{

    std::string name = "";

    std::string Content = XML::Parser::getElementContent(source);

    if (XML::Parser::elementExists(Content,"name")) {
                /*temp2 = getElement(temp,"name");
                name = getElementContent(temp2);*/ //would it be better to have just name = getElement(temp,"name");
                name = XML::Parser::getElementContent(XML::Parser::getElement(Content,"name"));
            }
            /**/

    return name;

}

Position Route::getPosition(std::string source)
{

    if (! XML::Parser::attributeExists(source,"lat")) throw std::domain_error("No 'lat' attribute.");
    if (! XML::Parser::attributeExists(source,"lon")) throw std::domain_error("No 'lon' attribute.");

    std::string lat = XML::Parser::getElementAttribute(source, "lat");
    std::string lon = XML::Parser::getElementAttribute(source, "lon");

    /*string rteptData = "";
    rteptData = getElementContent(rteptSource);*/
    std::string elevationData = XML::Parser::getElementContent(source);

    if (XML::Parser::elementExists(elevationData, "ele")) {
        /*temp2 = getElement(temp, "ele");
        ele = getElementContent(temp2);*/ //would it be better to have just ele = getElement(temp,"ele");
        std::string ele = XML::Parser::getElementContent(XML::Parser::getElement(elevationData, "ele"));

        Position foundPosition = Position(lat,lon,ele);
        return foundPosition;
    }
    else
    {
        Position foundPosition = Position(lat,lon);
        return foundPosition;

    }



}

void Route::setGranularity(metres granularity)
{
    bool implemented = false;
    assert(implemented);
}

bool Route::areSameLocation(const Position & p1, const Position & p2) const
{
    return (distanceBetween(p1,p2) < granularity);
}
