cmake_minimum_required(VERSION 3.10.0)

if(o2_WITH_FACEBOOK)
    add_subdirectory(facebookdemo)
endif(o2_WITH_FACEBOOK)

if(o2_WITH_GOOGLE)
    add_subdirectory(youtubedemo)
endif(o2_WITH_GOOGLE)

if(o2_WITH_TWITTER)
    add_subdirectory(twitterdemo)
endif(o2_WITH_TWITTER)

#if(o2_WITH_SPOTIFY)
#    add_subdirectory(spotifydemo)
#endif(o2_WITH_SPOTIFY)

if (o2_WITH_VIMEO)
    add_subdirectory(vimeodemo)
endif(o2_WITH_VIMEO)

if (o2_WITH_MSGRAPH)
    add_subdirectory(msgraphdemo)
    if (NOT o2_WITH_QT6) # Not yet completely updated to Qt6 compatibility, should not be too hard to do
        add_subdirectory(msgraphexternalinterceptordemo)
    endif (NOT o2_WITH_QT6)
endif(o2_WITH_MSGRAPH)
