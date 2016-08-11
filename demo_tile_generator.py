#!/usr/bin/python3

from struct import pack

NOFLY_TYPE_END             = 0
NOFLY_TYPE_CIRCLE          = 1
NOFLY_TYPE_POLYGON         = 2
NOFLY_TYPE_WARN_ONLY       = 4
NOFLY_TYPE_TIME_RESTRICTED = 8

ONE_MILE_IN_METERS = 1609

'''
    Arguments should be decimal-degrees for lat/lon and meters for radius.
    Returns a no-fly zone in binary format, as a Bytes object.
'''
def create_nofly_circle(latitude, longitude, radius, start_time = None, end_time = None, warn_only = False):
    args = []
    arg_types = "<"
    
    # write the nofly_type
    typ = NOFLY_TYPE_CIRCLE
    if start_time is not None and end_time is not None:
        typ |= NOFLY_TYPE_TIME_RESTRICTED
    if warn_only:
        typ |= NOFLY_TYPE_WARN_ONLY
    args.append(typ)
    arg_types += 'B'
    
    # if time is required, write it
    if start_time is not None and end_time is not None:
        args.append(int(start_time))
        args.append(int(end_time))
        arg_types += 'II'
    
    # write lat, lon, and radius (in meters)
    args.append(int(latitude * 10**7))
    args.append(int(longitude * 10**7))
    args.append(radius)
    arg_types += 'iiH'
    
    nofly_element = pack(arg_types, *args)
    return nofly_element

'''
    The latlon argument should be a list (indefinite size) of tuples, with each tuple containing a lat and a lon
'''
def create_nofly_polygon(latlon, start_time = None, end_time = None, warn_only = False):
    if len(latlon) < 3:
        raise Exception("Not enough points to define a Polygon")
    if len(latlon) > 255:
        raise Exception("Too many points to define a Polygon")

    args = []
    arg_types = "<"

    # write the nofly_type
    typ = NOFLY_TYPE_POLYGON
    if start_time is not None and end_time is not None:
        typ |= NOFLY_TYPE_TIME_RESTRICTED
    if warn_only:
        typ |= NOFLY_TYPE_WARN_ONLY
    args.append(typ)
    arg_types += 'B'

    # 1 byte to define number of coordinate pairs
    args.append(len(latlon))
    arg_types += 'B'

    # if time is required, write it
    if start_time is not None and end_time is not None:
        args.append(int(start_time))
        args.append(int(end_time))
        arg_types += 'II'
    
    # write all lat/lon pairs
    for latitude ,longitude in latlon:
        args.append(int(latitude * 10**7))
        args.append(int(longitude * 10**7))
        arg_types += 'ii'
    
    nofly_element = pack(arg_types, *args)
    return nofly_element

# This code is meant to be imported and then used. If run directly, it will just create a sample tile called 'tile.dat'.
if __name__ == "__main__":
    tilefile = open('tile42N72W.dat', 'wb')

    #NOTE: The radii used below are totally arbitrary, and are only meant as an example
    circles = []

    # Boston Logan International Airport. Warn within 5 miles, stay-out within 1 mile.
    circles.append(create_nofly_circle(42.366564, -71.016920, 5 * ONE_MILE_IN_METERS, warn_only = True))
    circles.append(create_nofly_circle(42.366564, -71.016920, ONE_MILE_IN_METERS, warn_only = False))

    # Hanscom Air Force Base - stay out within 3 miles
    circles.append(create_nofly_circle(42.465082,-71.288614, 3 * ONE_MILE_IN_METERS))

    # Mt. Auburn Hospital - Stay out 200 meters with a random added timeframe
    circles.append(create_nofly_circle(42.373885,-71.134095, 200, start_time = 1470871000, end_time = 1470880000))

    for circle in circles:
        tilefile.write(circle)
    
    # Fenway park (sports stadium). Defined by triangle.
    poly = create_nofly_polygon([(42.345406, -71.098240),(42.347143, -71.094453),(42.347048, -71.098766)])
    
    tilefile.write(poly)

    tilefile.write(bytes([NOFLY_TYPE_END]))
    tilefile.close()
