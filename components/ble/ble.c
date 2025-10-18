#include <stdio.h>
#include "esp_log.h"

#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "services/gap/ble_svc_gap.h"
#include "services/ans/ble_svc_ans.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "bleprph.h"

#define MAC2STR(a) (a)[5], (a)[4], (a)[3], (a)[2], (a)[1], (a)[0]
// #define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"

#define CONFIG_BLE_NAME "@crlcbrl"

static const char *TAG = "NimBLE";

void ble_store_config_init(void);
static int bleprph_gap_event(struct ble_gap_event *event, void *arg);
int gatt_svr_init(void);

/**
 * Logs information about a connection to the console.
 */
static void bleprph_print_conn_desc(struct ble_gap_conn_desc *desc)
{
    ESP_LOGI(TAG, "handle=%d", desc->conn_handle);
    ESP_LOGI(TAG, "\t our_ota_addr_type = %d our_ota_addr = " MACSTR,
             desc->our_ota_addr.type, MAC2STR(desc->our_ota_addr.val));

    ESP_LOGI(TAG, "\t our_id_addr_type = %d our_id_addr = " MACSTR,
             desc->our_id_addr.type, MAC2STR(desc->our_id_addr.val));

    ESP_LOGI(TAG, "\t peer_ota_addr_type = %d peer_ota_addr = " MACSTR,
             desc->peer_ota_addr.type, MAC2STR(desc->peer_ota_addr.val));

    ESP_LOGI(TAG, "\t peer_id_addr_type = %d peer_id_addr = " MACSTR,
             desc->peer_id_addr.type, MAC2STR(desc->peer_id_addr.val));

    ESP_LOGI(TAG, "\t conn_itvl=%d conn_latency=%d supervision_timeout=%d "
                  "encrypted=%d authenticated=%d bonded=%d\n",
             desc->conn_itvl, desc->conn_latency,
             desc->supervision_timeout,
             desc->sec_state.encrypted,
             desc->sec_state.authenticated,
             desc->sec_state.bonded);
}

/**
 * Enables advertising with the following parameters:
 *     o General discoverable mode.
 *     o Undirected connectable mode.
 */
static void bleprph_advertise(void)
{
    struct ble_gap_adv_params adv_params = {0};
    struct ble_hs_adv_fields fields = {0};
    const char *name;
    int rc;

    /* Advertise two flags:
     *     o Discoverability in forthcoming advertisement (general)
     *     o BLE-only (BR/EDR unsupported).
     */
    // BLE_HS_ADV_F_DISC_LTD (Limited Discoverable Mode)
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /* Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO; // ESP_PWR_LVL_P9

    name = ble_svc_gap_device_name();
    fields.name = (uint8_t *)name;
    fields.name_len = strlen(name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "Error setting advertisement data; rc=%d\n", rc);
        return;
    }

    /* Begin advertising. */
    // BLE_GAP_CONN_MODE_NON(non - connectable)
    // BLE_GAP_CONN_MODE_DIR(directed - connectable)
    // BLE_GAP_CONN_MODE_UND(undirected - connectable)
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;

    // BLE_GAP_DISC_MODE_NON (non-discoverable).
    // BLE_GAP_DISC_MODE_LTD(limited - discoverable).
    // BLE_GAP_DISC_MODE_GEN(general - discoverable)
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(0, NULL, BLE_HS_FOREVER, &adv_params, bleprph_gap_event, NULL);
    if (rc != 0)
    {
        ESP_LOGE(TAG, "Error enabling advertisement; rc=%d\n", rc);
        return;
    }
}

/**
 * The nimble host executes this callback when a GAP event occurs.  The
 * application associates a GAP event callback with each connection that forms.
 * bleprph uses the same callback for all connections.
 *
 * @param event                 The type of event being signalled.
 * @param ctxt                  Various information pertaining to the event.
 * @param arg                   Application-specified argument; unused by
 *                                  bleprph.
 *
 * @return                      0 if the application successfully handled the
 *                                  event; nonzero on failure.  The semantics
 *                                  of the return code is specific to the
 *                                  particular GAP event being signalled.
 */
static int bleprph_gap_event(struct ble_gap_event *event, void *arg)
{
    struct ble_gap_conn_desc desc;
    int rc;

    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed. */
        ESP_LOGW(TAG, "Connection %s; status=%d ",
                 event->connect.status == 0 ? "established" : "failed",
                 event->connect.status);

        if (event->connect.status == 0)
        {
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);
            assert(rc == 0);
            bleprph_print_conn_desc(&desc);
        }

        if (event->connect.status != 0)
        {
            /* Connection failed; resume advertising. */
            bleprph_advertise();
        }
        return 0;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGW(TAG, "BLE disconnected! Reason=%d ", event->disconnect.reason);
        bleprph_print_conn_desc(&event->disconnect.conn);
        bleprph_advertise();
        return 0;

    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        ESP_LOGI(TAG, "Connection updated; status=%d ", event->conn_update.status);
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        assert(rc == 0);
        bleprph_print_conn_desc(&desc);
        return 0;

    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGV(TAG, "advertise complete; reason=%d", event->adv_complete.reason);
        bleprph_advertise();
        return 0;

    case BLE_GAP_EVENT_ENC_CHANGE:
        /* Encryption has been enabled or disabled for this connection. */
        ESP_LOGW(TAG, "Encryption change event; status=%d ", event->enc_change.status);
        rc = ble_gap_conn_find(event->enc_change.conn_handle, &desc);
        assert(rc == 0);
        bleprph_print_conn_desc(&desc);
        return 0;

    case BLE_GAP_EVENT_NOTIFY_TX:
        ESP_LOGW(TAG, "Notify_tx event; conn_handle=%d attr_handle=%d "
                      "status=%d is_indication=%d",
                 event->notify_tx.conn_handle,
                 event->notify_tx.attr_handle,
                 event->notify_tx.status,
                 event->notify_tx.indication);
        return 0;

    case BLE_GAP_EVENT_SUBSCRIBE:
        ESP_LOGI(TAG, "Subscribe event; conn_handle=%d attr_handle=%d "
                      "reason=%d prevn=%d curn=%d previ=%d curi=%d\n",
                 event->subscribe.conn_handle,
                 event->subscribe.attr_handle,
                 event->subscribe.reason,
                 event->subscribe.prev_notify,
                 event->subscribe.cur_notify,
                 event->subscribe.prev_indicate,
                 event->subscribe.cur_indicate);
        return 0;

    case BLE_GAP_EVENT_MTU:
        ESP_LOGI(TAG, "MTU update event; conn_handle=%d cid=%d mtu=%d\n",
                 event->mtu.conn_handle,
                 event->mtu.channel_id,
                 event->mtu.value);
        return 0;

    case BLE_GAP_EVENT_REPEAT_PAIRING:
        /* We already have a bond with the peer, but it is attempting to
         * establish a new secure link.  This app sacrifices security for
         * convenience: just throw away the old bond and accept the new link.
         */
        ESP_LOGI(TAG, "Repeat pairing   event; conn_handle=%d\n", event->repeat_pairing.conn_handle);

        /* Delete the old bond. */
        rc = ble_gap_conn_find(event->repeat_pairing.conn_handle, &desc);
        assert(rc == 0);
        ble_store_util_delete_peer(&desc.peer_id_addr);

        /* Return BLE_GAP_REPEAT_PAIRING_RETRY to indicate that the host should
         * continue with the pairing operation.
         */
        return BLE_GAP_REPEAT_PAIRING_RETRY;

    case BLE_GAP_EVENT_PASSKEY_ACTION:
        ESP_LOGW(TAG, "Pass key action event!\n");
        return 0;

    case BLE_GAP_EVENT_AUTHORIZE:
        ESP_LOGW(TAG, "authorize event: conn_handle=%d attr_handle=%d is_read=%d",
                 event->authorize.conn_handle,
                 event->authorize.attr_handle,
                 event->authorize.is_read);

        /* The default behaviour for the event is to reject authorize request */
        event->authorize.out_response = BLE_GAP_AUTHORIZE_REJECT;
        return 0;
    }

    return 0;
}

static void bleprph_on_sync(void)
{
    int rc;
    rc = ble_hs_util_ensure_addr(0);

    assert(rc == 0);

    /* Begin advertising. */
    bleprph_advertise();
}

void bleprph_host_task(void *param)
{
    ESP_LOGV(TAG, "BLE Host Task Started");
    /* This function will return only when nimble_port_stop() is executed */
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void configure_ble()
{
    int rc;
    esp_err_t ret;
    struct ble_hs_cfg;

    ret = nimble_port_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to init nimble %d ", ret);
        return;
    }

    /* Initialize the NimBLE host configuration. */
    // ble_hs_cfg.reset_cb = bleprph_on_reset;
    ble_hs_cfg.sync_cb = bleprph_on_sync;
    // ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    ble_hs_cfg.sm_io_cap = BLE_SM_IO_CAP_NO_IO;
    ble_hs_cfg.sm_bonding = 0;
    ble_hs_cfg.sm_mitm = 1;
    ble_hs_cfg.sm_sc = 1; // Secure conection

    rc = gatt_svr_init();
    assert(rc == 0);

    /* Set the default device name. */
    rc = ble_svc_gap_device_name_set(CONFIG_BLE_NAME);
    assert(rc == 0);

    /*Lower TX power*/
    // ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_DEFAULT, ESP_PWR_LVL_N12));
    // ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_ADV, ESP_PWR_LVL_N12));
    // ESP_ERROR_CHECK(esp_ble_tx_power_set(ESP_BLE_PWR_TYPE_CONN_HDL0, ESP_PWR_LVL_N12));

    /* XXX Need to have template for store */
    // ble_store_config_init();

    nimble_port_freertos_init(bleprph_host_task);
}