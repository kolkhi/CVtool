/** @file uavv_klv.h
    @brief Header file for UAVV KLV Data Set
    @ingroup client_api
 */


#ifndef UAVV_KLV_H
#define UAVV_KLV_H


#ifdef __cplusplus
extern "C" {
#endif


#define UAVV_KLV_FALSE      0       ///< Result for false value
#define UAVV_KLV_TRUE       1       ///< Result for true value


/// @brief Handle to a KLV data set
typedef void* UAVV_KLV;

/// @brief KLV keys
typedef enum
{
    UAVV_KLV_CHECKSUM,
    UAVV_KLV_UNIX_TIME_STAMP,
    UAVV_KLV_MISSION_ID,
    UAVV_KLV_PLATFORM_TAIL_NUMBER,
    UAVV_KLV_PLATFORM_HEADING_ANGLE,
    UAVV_KLV_PLATFORM_PITCH_ANGLE,
    UAVV_KLV_PLATFORM_ROLL_ANGLE,
    UAVV_KLV_PLATFORM_TRUE_AIRSPEED,
    UAVV_KLV_PLATFORM_INDICATED_AIRSPEED,
    UAVV_KLV_PLATFORM_DESIGNATION,
    UAVV_KLV_IMAGE_SOURCE_SENSOR,
    UAVV_KLV_IMAGE_COORDINATE_SYSTEM,
    UAVV_KLV_SENSOR_LATITUDE,
    UAVV_KLV_SENSOR_LONGITUDE,
    UAVV_KLV_SENSOR_TRUE_ALTITUDE,
    UAVV_KLV_SENSOR_HORIZONTAL_FIELD_OF_VIEW,
    UAVV_KLV_SENSOR_VERTICAL_FIELD_OF_VIEW,
    UAVV_KLV_SENSOR_RELATIVE_AZIMUTH_ANGLE,
    UAVV_KLV_SENSOR_RELATIVE_ELEVATION_ANGLE,
    UAVV_KLV_SENSOR_RELATIVE_ROLL_ANGLE,
    UAVV_KLV_SLANT_RANGE,
    UAVV_KLV_TARGET_WIDTH,
    UAVV_KLV_FRAME_CENTER_LATITUDE,
    UAVV_KLV_FRAME_CENTER_LONGITUDE,
    UAVV_KLV_FRAME_CENTER_ELEVATION,
    UAVV_KLV_OFFSET_CORNER_LATITUDE_POINT_1,
    UAVV_KLV_OFFSET_CORNER_LONGITUDE_POINT_1,
    UAVV_KLV_OFFSET_CORNER_LATITUDE_POINT_2,
    UAVV_KLV_OFFSET_CORNER_LONGITUDE_POINT_2,
    UAVV_KLV_OFFSET_CORNER_LATITUDE_POINT_3,
    UAVV_KLV_OFFSET_CORNER_LONGITUDE_POINT_3,
    UAVV_KLV_OFFSET_CORNER_LATITUDE_POINT_4,
    UAVV_KLV_OFFSET_CORNER_LONGITUDE_POINT_4,
    UAVV_KLV_ICING_DETECTED,
    UAVV_KLV_WIND_DIRECTION,
    UAVV_KLV_WIND_SPEED,
    UAVV_KLV_STATIC_PRESSURE,
    UAVV_KLV_DENSITY_ALTITUDE,
    UAVV_KLV_OUTSIDE_AIR_TEMPERATURE,
    UAVV_KLV_TARGET_LOCATION_LATITUDE,
    UAVV_KLV_TARGET_LOCATION_LONGITUDE,
    UAVV_KLV_TARGET_LOCATION_ELEVATION,
    UAVV_KLV_TARGET_TRACK_GATE_WIDTH,
    UAVV_KLV_TARGET_TRACK_GATE_HEIGHT,
    UAVV_KLV_TARGET_ERROR_ESTIMATE_CE90,
    UAVV_KLV_TARGET_ERROR_ESTIMATE_LE90,
    UAVV_KLV_GENERIC_FLAG_DATA_01,
    UAVV_KLV_SECURITY_LOCAL_METADATA_SET,
    UAVV_KLV_DIFFERENTIAL_PRESSURE,
    UAVV_KLV_PLATFORM_ANGLE_OF_ATTACK,
    UAVV_KLV_PLATFORM_VERTICAL_SPEED,
    UAVV_KLV_PLATFORM_SIDESLIP_ANGLE,
    UAVV_KLV_AIRFIELD_BAROMETRIC_PRESSURE,
    UAVV_KLV_AIRFIELD_ELEVATION,
    UAVV_KLV_RELATIVE_HUMIDITY,
    UAVV_KLV_PLATFORM_GROUND_SPEED,
    UAVV_KLV_GROUND_RANGE,
    UAVV_KLV_PLATFORM_FUEL_REMAINING,
    UAVV_KLV_PLATFORM_CALL_SIGN,
    UAVV_KLV_WEAPON_LOAD,
    UAVV_KLV_WEAPON_FIRED,
    UAVV_KLV_LASER_PRF_CODE,
    UAVV_KLV_SENSOR_FIELD_OF_VIEW_NAME,
    UAVV_KLV_PLATFORM_MAGNETIC_HEADING,
    UAVV_KLV_UAS_LS_VERSION_NUMBER,
    UAVV_KLV_TARGET_LOCATION_COVARIANCE_MATRIX,
    UAVV_KLV_ALTERNATE_PLATFORM_LATITUDE,
    UAVV_KLV_ALTERNATE_PLATFORM_LONGITUDE,
    UAVV_KLV_ALTERNATE_PLATFORM_ALTITUDE,
    UAVV_KLV_ALTERNATE_PLATFORM_NAME,
    UAVV_KLV_ALTERNATE_PLATFORM_HEADING,
    UAVV_KLV_EVENT_START_TIME_UTC,
    UAVV_KLV_RVT_LOCAL_SET,
    UAVV_KLV_VMTI_DATA_SET,
    UAVV_KLV_SENSOR_ELLIPSOID_HEIGHT,
    UAVV_KLV_ALTERNATE_PLATFORM_ELLIPSOID_HEIGHT,
    UAVV_KLV_OPERATIONAL_MODE,
    UAVV_KLV_FRAME_CENTER_HEIGHT_ABOVE_ELLIPSOID,
    UAVV_KLV_SENSOR_NORTH_VELOCITY,
    UAVV_KLV_SENSOR_EAST_VELOCITY,
    UAVV_KLV_IMAGE_HORIZON_PIXEL_PACK,
    UAVV_KLV_CORNER_LATITUDE_POINT_1_FULL,
    UAVV_KLV_CORNER_LONGITUDE_POINT_1_FULL,
    UAVV_KLV_CORNER_LATITUDE_POINT_2_FULL,
    UAVV_KLV_CORNER_LONGITUDE_POINT_2_FULL,
    UAVV_KLV_CORNER_LATITUDE_POINT_3_FULL,
    UAVV_KLV_CORNER_LONGITUDE_POINT_3_FULL,
    UAVV_KLV_CORNER_LATITUDE_POINT_4_FULL,
    UAVV_KLV_CORNER_LONGITUDE_POINT_4_FULL,
    UAVV_KLV_PLATFORM_PITCH_ANGLE_FULL,
    UAVV_KLV_PLATFORM_ROLL_ANGLE_FULL,
    UAVV_KLV_PLATFORM_ANGLE_OF_ATTACK_FULL,
    UAVV_KLV_PLATFORM_SIDESLIP_ANGLE_FULL,
    UAVV_KLV_MIIS_CORE_IDENTIFIER,
    UAVV_KLV_SAR_MOTION_IMAGERY_METADATA,
    UAVV_KLV_TARGET_WIDTH_EXTENDED,
    MAX_UAVV_KLV_KEY
} uavv_klv_key_t;


/// @name Memory manager
/// @{

/** @brief Creates a KLV data set.
    @return Handle to a new KLV data set.  This should be destroyed using
            uavv_klv_destroy() after use.
  */
UAVV_KLV uavv_klv_create();

/** @brief Copies a KLV data set.
    @return Handle to a new KLV data set.  This should be destroyed using
            uavv_klv_destroy() after use.
    @param src  Handle to the source KLV data set.
  */
UAVV_KLV uavv_klv_copy(const UAVV_KLV src);

/** @brief Destroys a KLV data set.
    @param klv  Handle to a KLV data set.
  */
void uavv_klv_destroy(UAVV_KLV klv);

/// @}


/// @name Item enumerators
/// @{

/** @brief Obtains the total number of items in a KLV data set.
    @return The number of items in the KLV data set.
    @param klv  Handle to a KLV data set.
  */
int uavv_klv_size(const UAVV_KLV klv);

/** @brief Obtains the key of a KLV item at a particular offset.
    @return The key of item at position @p offset.
    @return @p MAX_UAVV_KLV_KEY if the offset is invalid.
    @param klv  Handle to a KLV data set.
    @param offset   Zero-based offset of the KLV item.  This number must
                    be less than the value returned by uavv_klv_size().
  */
uavv_klv_key_t uavv_klv_item(const UAVV_KLV klv, int offset);

/** @brief Retrieves the name of a KLV item.
    @return A human-readable name of the KLV item.
    @param key  Key of the KLV item being named.
  */
const char* uavv_klv_item_name(uavv_klv_key_t key);

/// @}


/// @name Item getters
/// @{

/** @brief Gets the UTC timetamp of a KLV data set.
    @return The UTC timestamp of the KLV data set.
    @param klv  Handle to a KLV data set.
 */
long long uavv_klv_get_timestamp(const UAVV_KLV klv);

/** @brief Checkes whether a KLV item exists.
    @return @p UAVV_KLV_TRUE if the KLV item exists.
    @return @p UAVV_KLV_FALSE if the KLV item does not exist.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being checked.
  */
int uavv_klv_has_key(const UAVV_KLV klv, uavv_klv_key_t key);

/** @brief Checkes whether a KLV item is in an error state.
    @return @p UAVV_KLV_TRUE if the KLV item is in an error state.
    @return @p UAVV_KLV_FALSE if the KLV item is not in an error state.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being checked.
  */
int uavv_klv_get_error(const UAVV_KLV klv, uavv_klv_key_t key);

/** @brief Gets the value of a KLV item as a string.
    @return The value of the KLV item if it exists.  The string is valid until
            the KLV data set is next modified.
    @return An empty string if the KLV item does not exist.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being retrieved.
  */
const char* uavv_klv_get_str(const UAVV_KLV klv, uavv_klv_key_t key);

/** @brief Gets the value of a KLV item as an integer.
    @return The value of the KLV item.
    @return @p 0 if the KLV item does not exist.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being retrieved.
  */
int uavv_klv_get_int(const UAVV_KLV klv, uavv_klv_key_t key);

/** @brief Gets the value of a KLV item as a floating point number.
    @return The value of the KLV item.
    @return @p 0 if the KLV item does not exist.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being retrieved.
 */
double uavv_klv_get_float(const UAVV_KLV klv, uavv_klv_key_t key);

/// @}


/// @name Item setters
/// @{

/** @brief Sets the UTC timetamp of a KLV data set.
    @param klv          Handle to a KLV data set.
    @param timestamp    New UTC timestamp.
 */
void uavv_klv_set_timestamp(UAVV_KLV klv, long long timestamp);

/** @brief Sets a KLV item to be in an error state.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being set.
  */
void uavv_klv_set_error(UAVV_KLV klv, uavv_klv_key_t key);

/** @brief Sets a string as the value of a KLV item.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being set.
    @param val  New value of the item.  The string value is copied by the KLV
                data set.
  */
void uavv_klv_set_str(UAVV_KLV klv, uavv_klv_key_t key, const char* val);

/** @brief Sets an integer as the value of a KLV item.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being set.
    @param val  New value of the item.
  */
void uavv_klv_set_int(UAVV_KLV klv, uavv_klv_key_t key, int val);

/** @brief Sets a floating point number as the value of a KLV item.
    @param klv  Handle to a KLV data set.
    @param key  Key of the KLV item being set.
    @param val  New value of the item.
 */
void uavv_klv_set_float(UAVV_KLV klv, uavv_klv_key_t key, double val);

/// @}


#ifdef __cplusplus
}
#endif


#endif  /* UAVV_KLV_H */


/* vim: set cindent tabstop=8 softtabstop=4 expandtab shiftwidth=4: */
