#ifndef BLE_CHORD_H__
#define BLE_CHORD_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

/**@brief   Macro for defining a ble_hrs instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_CHORD_DEF(_name)                                                                          \
static ble_chord_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_HRS_BLE_OBSERVER_PRIO,                                                     \
                     ble_chord_on_ble_evt, &_name)

#define CHORD_SERVICE_UUID_BASE         { 0x44, 0x4e, 0xa3, 0xb6, 0x41, 0x61, 0x4d, 0x69, \
	                                       0xa7, 0x51, 0x89, 0xeb, 0xa8, 0x8d, 0xd0, 0x5e }

#define CHORD_SERVICE_UUID               0x1400
#define CHORD_VALUE_CHAR_UUID            0x1401
																					
/**@brief Custom Service event type. */
typedef enum
{
    BLE_CHORD_EVT_NOTIFICATION_ENABLED,                             /**< Chord value notification enabled event. */
    BLE_CHORD_EVT_NOTIFICATION_DISABLED,                             /**< Chord value notification disabled event. */
    BLE_CHORD_EVT_DISCONNECTED,
    BLE_CHORD_EVT_CONNECTED
} ble_chord_evt_type_t;

/**@brief Chord Service event. */
typedef struct
{
    ble_chord_evt_type_t evt_type;                                  /**< Type of event. */
} ble_chord_evt_t;

// Forward declaration of the ble_chord_t type.
typedef struct ble_chord_s ble_chord_t;


/**@brief Custom Service event handler type. */
typedef void (*ble_chord_evt_handler_t) (ble_chord_t * p_bas, ble_chord_evt_t * p_evt);

/**@brief Battery Service init structure. This contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_chord_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint8_t                       initial_chord_value;           /**< Initial chord value */
    ble_srv_cccd_security_mode_t  chord_value_char_attr_md;     /**< Initial security level for Chord characteristics attribute */
} ble_chord_init_t;

/**@brief Custom Service structure. This contains various status information for the service. */
struct ble_chord_s
{
    ble_chord_evt_handler_t         evt_handler;                    /**< Event handler to be called for handling events in the Custom Service. */
    uint16_t                      service_handle;                 /**< Handle of Custom Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t      chord_value_handles;           /**< Handles related to the Custom Value characteristic. */
    uint16_t                      conn_handle;                    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                       uuid_type; 
};

/**@brief Function for initializing the Custom Service.
 *
 * @param[out]  p_chord       Custom Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_chord_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_chord_init(ble_chord_t * p_chord, const ble_chord_init_t * p_chord_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Battery Service.
 *
 * @note 
 *
 * @param[in]   p_chord      Custom Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_chord_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for updating the custom value.
 *
 * @details The application calls this function when the cutom value should be updated. If
 *          notification has been enabled, the custom value characteristic is sent to the client.
 *
 * @note 
 *       
 * @param[in]   p_bas          Chord Service structure.
 * @param[in]   Chord value 
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_chord_chord_value_update(ble_chord_t * p_chord, uint8_t chord_value);

#endif // BLE_CHORD_H__
