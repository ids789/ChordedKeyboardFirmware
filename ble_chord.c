#include "sdk_common.h"
#include "ble_chord.h"
#include <string.h>
#include "ble_srv_common.h"
#include "boards.h"
#include "nrf_log.h"

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_chord       Chord Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_chord_t * p_chord, ble_evt_t const * p_ble_evt)
{
    p_chord->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;

    ble_chord_evt_t evt;

    evt.evt_type = BLE_CHORD_EVT_CONNECTED;

    p_chord->evt_handler(p_chord, &evt);
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_chord       Chord Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_chord_t * p_chord, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_chord->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    ble_chord_evt_t evt;

    evt.evt_type = BLE_CHORD_EVT_DISCONNECTED;

    p_chord->evt_handler(p_chord, &evt);
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_chord       Chord Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_chord_t * p_chord, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;

    // Check if the Chord value CCCD is written to and that the value is the appropriate length, i.e 2 bytes.
    if ((p_evt_write->handle == p_chord->chord_value_handles.cccd_handle)
        && (p_evt_write->len == 2)
       )
    {
        // CCCD written, call application event handler
        if (p_chord->evt_handler != NULL)
        {
            ble_chord_evt_t evt;

            if (ble_srv_is_notification_enabled(p_evt_write->data))
            {
                evt.evt_type = BLE_CHORD_EVT_NOTIFICATION_ENABLED;
            }
            else
            {
                evt.evt_type = BLE_CHORD_EVT_NOTIFICATION_DISABLED;
            }
            // Call the application event handler.
            p_chord->evt_handler(p_chord, &evt);
        }
    }

}

void ble_chord_on_ble_evt( ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_chord_t * p_chord = (ble_chord_t *) p_context;
    
    NRF_LOG_INFO("BLE event received. Event type = %d\r\n", p_ble_evt->header.evt_id); 
    if (p_chord == NULL || p_ble_evt == NULL)
    {
        return;
    }
    
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_chord, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_chord, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_chord, p_ble_evt);
            break;

        default:
            break;
    }
}

/**@brief Function for adding the Chord Value characteristic.
 *
 * @param[in]   p_chord        Battery Service structure.
 * @param[in]   p_chord_init   Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
static uint32_t chord_value_char_add(ble_chord_t * p_chord, const ble_chord_init_t * p_chord_init)
{
    uint32_t            err_code;
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    // Add Chord Value characteristic
    memset(&cccd_md, 0, sizeof(cccd_md));

    //  Read  operation on cccd should be possible without authentication.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&cccd_md.write_perm);
    
    cccd_md.write_perm = p_chord_init->chord_value_char_attr_md.cccd_write_perm;
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1; 
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md; 
    char_md.p_sccd_md         = NULL;
		
    ble_uuid.type = p_chord->uuid_type;
    ble_uuid.uuid = CHORD_VALUE_CHAR_UUID;

    memset(&attr_md, 0, sizeof(attr_md));

    attr_md.read_perm  = p_chord_init->chord_value_char_attr_md.read_perm;
    attr_md.write_perm = p_chord_init->chord_value_char_attr_md.write_perm;
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = sizeof(uint8_t);

    err_code = sd_ble_gatts_characteristic_add(p_chord->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_chord->chord_value_handles);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

uint32_t ble_chord_init(ble_chord_t * p_chord, const ble_chord_init_t * p_chord_init)
{
    if (p_chord == NULL || p_chord_init == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_chord->evt_handler               = p_chord_init->evt_handler;
    p_chord->conn_handle               = BLE_CONN_HANDLE_INVALID;

    // Add Chord Service UUID
    ble_uuid128_t base_uuid = {CHORD_SERVICE_UUID_BASE};
    err_code =  sd_ble_uuid_vs_add(&base_uuid, &p_chord->uuid_type);
    VERIFY_SUCCESS(err_code);
    
    ble_uuid.type = p_chord->uuid_type;
    ble_uuid.uuid = CHORD_SERVICE_UUID;

    // Add the Chord Service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_chord->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add Chord Value characteristic
    return chord_value_char_add(p_chord, p_chord_init);
}

uint32_t ble_chord_chord_value_update(ble_chord_t * p_chord, uint8_t chord_value)
{
    NRF_LOG_INFO("In ble_chord_chord_value_update. \r\n"); 
    if (p_chord == NULL)
    {
        return NRF_ERROR_NULL;
    }

    uint32_t err_code = NRF_SUCCESS;
    ble_gatts_value_t gatts_value;

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = sizeof(uint8_t);
    gatts_value.offset  = 0;
    gatts_value.p_value = &chord_value;

    // Update database.
    err_code = sd_ble_gatts_value_set(p_chord->conn_handle,
                                      p_chord->chord_value_handles.value_handle,
                                      &gatts_value);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Send value if connected and notifying.
    if ((p_chord->conn_handle != BLE_CONN_HANDLE_INVALID)) 
    {
        ble_gatts_hvx_params_t hvx_params;

        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_chord->chord_value_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = gatts_value.offset;
        hvx_params.p_len  = &gatts_value.len;
        hvx_params.p_data = gatts_value.p_value;

        err_code = sd_ble_gatts_hvx(p_chord->conn_handle, &hvx_params);
        NRF_LOG_INFO("sd_ble_gatts_hvx result: %x. \r\n", err_code); 
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
        NRF_LOG_INFO("sd_ble_gatts_hvx result: NRF_ERROR_INVALID_STATE. \r\n"); 
    }


    return err_code;
}
