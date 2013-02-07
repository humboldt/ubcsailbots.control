'''
Created on Jan 20, 2013

@author: joshandrews
'''
import math
import control.datatype.datatypes as datatype
from control.parser import parsing
from os import path
from control import StaticVars as sVars

EARTH_RADIUS = 6378140

#returns gpscoordinate distance in meters away from starting point.
#positive yDist = North, positive xDist = East
def GPSDistAway(coord, yDist, xDist):
    result = datatype.GPSCoordinate()
    result.long = coord.long + (180.0/math.pi)*(float(xDist)/EARTH_RADIUS)/math.cos(math.radians(coord.lat))
    result.lat = coord.lat + (180.0/math.pi)*(float(yDist)/EARTH_RADIUS)
    return result


#Returns the distance in metres
def distBetweenTwoCoords(coord1, coord2):
    dLongRad = math.radians(coord1.long - coord2.long)
    dLatRad = math.radians(coord1.lat - coord2.lat)
    latRad1 = math.radians(coord1.lat)
    latRad2 = math.radians(coord2.lat)
    
    a = math.pow(math.sin(dLatRad/2),2) + math.cos(latRad1)*math.cos(latRad2)*math.pow(math.sin(dLongRad/2),2)
    c = 2*math.atan2(math.sqrt(a),math.sqrt(1-a))
    return EARTH_RADIUS*c

#Returns the angle in degrees
def angleBetweenTwoCoords(sourceCoord, destCoord):
    GPSCoord = datatype.GPSCoordinate
    
    if(sourceCoord.lat > destCoord.lat):
        GPSCoord.lat = sourceCoord.lat
        GPSCoord.long = destCoord.long
    
    elif(sourceCoord.lat < destCoord.lat):
        GPSCoord.lat = destCoord.lat
        GPSCoord.long = sourceCoord.long
    
    elif(sourceCoord.long < destCoord.long):
        return 90
    
    elif(sourceCoord.long > destCoord.long):
        return -90
    
    else:
        return None
    
    
    distBtwnCoords = distBetweenTwoCoords(sourceCoord, destCoord)
    distSin = distBetweenTwoCoords(destCoord, GPSCoord)
    
    angle = math.asin(distSin/distBtwnCoords)*180/math.pi
    
    if(sourceCoord.lat < destCoord.lat):
        if(sourceCoord.long < destCoord.long):
            return datatype.Angle(angle)
        elif(sourceCoord.long > destCoord.long):
            angle = -angle
            return datatype.Angle(angle)
        else:
            return datatype.Angle(0)        
    else:
        if(sourceCoord.long < destCoord.long):
            angle = 90+angle
            return datatype.Angle(angle)
        elif(sourceCoord.long > destCoord.long):
            angle = -90-angle
            return datatype.Angle(angle)
        else:
            return datatype.Angle(180)

#Determines whether the waypoint can be reached with our current coordinates
#Returns 1 if waypoint can't be reached
#Returns 0 if waypoint can be reached
def isWPNoGo (AWA, hog, dest, sog, GPS):
    AWAList = parsing.parse(path.join(path.dirname(__file__), 'AWA'))
    if(sog < sVars.SPEED_AFFECTION_THRESHOLD):
        if(hog-AWA-45 < angleBetweenTwoCoords(GPS,dest).degrees() and angleBetweenTwoCoords(GPS,dest).degrees() < hog-AWA+45):
            return 1
        else:
            return 0
    else:
        AWAindex = searchIndex(AWA, AWAList)
        return 0

def getTrueWindAngle(awa, sog):
    return 0

#Only works with tables with 4 columns!!!!!        
def searchIndex(number, list1):
    big_list = list()
    indcol_list = list()
    
    for i in range(len(list1)):
        for j in range(len(list1[i])):
            big_list.append(list1[i][j])    
    
    for n in range(len(big_list)):
        if( math.fabs(big_list[n]-number) <= sVars.AWA_THRESHOLD ):
            index = math.floor(n/4)
            column = n%4
            small_list = [index,column]
            indcol_list.append(small_list)
            
    return indcol_list
    
        
        
    
                    