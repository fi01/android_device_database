#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/system_properties.h>
#include "sqlite3.h"
#include "device_database.h"

#define DEVICE_DATABASE_FILE    "device.db"

#define SQL_SELECT_DEVICE_SUPPORTED_DEVICES \
  "select device_id, check_property_name, check_property_value from supported_devices " \
  "where (device = ?) and (build_id = ?);"

#define SQL_SELECT_DEVICE_ADDRESS \
  "select value from device_address where (device_id = ?) and (name = ?);"

#define IS_SQL_ERROR(rc) ((rc) != SQLITE_OK && (rc) != SQLITE_DONE && (rc) != SQLITE_ROW)


static sqlite3 *db;

static void
close_database(void)
{
  int i;
  int rc;

  if (!db) {
    return;
  }

  for (i = 0; i < 300; i++) {
    rc = sqlite3_close(db);

    if (rc == SQLITE_BUSY) {
      usleep(10000);
      continue;
    }

    break;
  }

  if (IS_SQL_ERROR(rc)) {
    printf("%s(%d)\n", sqlite3_errmsg(db), sqlite3_errcode(db));
  }

  //printf("database is closed.\n");
}

static bool
init_database(void)
{
  int rc;

  if (db != NULL) {
    return true;
  }

  if (access(DEVICE_DATABASE_FILE, R_OK | W_OK)) {
    printf("FATAL ERROR: DB file open failed.\n");
    printf("Make sure install \"" DEVICE_DATABASE_FILE "\" from device_database!\n");
    exit(1);
  }

  rc = sqlite3_open(DEVICE_DATABASE_FILE, &db);
  if (rc) {
    printf("Error = %d: sqlite3_open(): failed\n", rc);

    db = NULL;
    return false;
  }

  atexit(close_database);

  return true;
}

static int
execute_sql(sqlite3_stmt *st)
{
  int rc;

  while (true) {
    rc = sqlite3_step(st);
    if (rc == SQLITE_DONE) {
      break;
    }

    if (rc == SQLITE_BUSY) {
      continue;
    }

    if (rc == SQLITE_ROW) {
      break;
    }

    break;
  }

  return rc;
}

device_id_t
detect_device(void)
{
  char device[PROP_VALUE_MAX];
  char build_id[PROP_VALUE_MAX];
  device_id_t device_id;
  sqlite3_stmt *st;
  int rc;
  int i;

  if (!init_database()) {
    return DEVICE_NOT_SUPPORTED;
  }

  __system_property_get("ro.product.model", device);
  __system_property_get("ro.build.display.id", build_id);

  device_id = DEVICE_NOT_SUPPORTED;

  rc = sqlite3_prepare(db, SQL_SELECT_DEVICE_SUPPORTED_DEVICES , -1, &st, NULL);

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_reset(st);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_bind_text(st, 1, device, -1, SQLITE_STATIC);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_bind_text(st, 2, build_id, -1, SQLITE_STATIC);
  }

  if (!IS_SQL_ERROR(rc)) {
    for (rc = execute_sql(st); rc == SQLITE_ROW; rc = execute_sql(st)) {
      const char *check_name;
      const char *check_value;

      device_id = sqlite3_column_int(st, 0);
      check_name = sqlite3_column_text(st, 1);
      check_value = sqlite3_column_text(st, 2);

      if (!check_name && !check_value) {
        break;
      }

      if (check_name && check_value) {
	char property_value[PROP_VALUE_MAX];

	__system_property_get(check_name, property_value);

	if (strcmp(property_value, check_value) == 0) {
	  break;
	}
      }

      device_id = DEVICE_NOT_SUPPORTED;
    }
  }

  if (IS_SQL_ERROR(rc)) {
    printf("%s(%d)\n", sqlite3_errmsg(db), sqlite3_errcode(db));
    sqlite3_finalize(st);

    return device_id;
  }

  sqlite3_finalize(st);
  return device_id;
}

unsigned long int
device_get_symbol_address(device_symbol_t symbol)
{
  device_id_t device_id;
  sqlite3_stmt *st;
  int rc;
  unsigned long int value;

  device_id = detect_device();
  if (device_id == DEVICE_NOT_SUPPORTED) {
    return 0;
  }

  value = 0;

  rc = sqlite3_prepare(db, SQL_SELECT_DEVICE_ADDRESS , -1, &st, NULL);

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_reset(st);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_bind_int(st, 1, device_id);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_bind_text(st, 2, symbol, -1, SQLITE_STATIC);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = execute_sql(st);
  }

  if (!IS_SQL_ERROR(rc)) {
    value = sqlite3_column_int(st, 0);
  }

  if (IS_SQL_ERROR(rc)) {
    printf("%s(%d)\n", sqlite3_errmsg(db), sqlite3_errcode(db));
  }

  sqlite3_finalize(st);

  return value;
}

void
print_reason_device_not_supported(void)
{
  char device[PROP_VALUE_MAX];
  char build_id[PROP_VALUE_MAX];
  const char *check_name;
  sqlite3_stmt *st;
  int rc;
  int i;

  if (!init_database()) {
    return;
  }

  __system_property_get("ro.product.model", device);
  __system_property_get("ro.build.display.id", build_id);

  rc = sqlite3_prepare(db, SQL_SELECT_DEVICE_SUPPORTED_DEVICES , -1, &st, NULL);

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_reset(st);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_bind_text(st, 1, device, -1, SQLITE_STATIC);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = sqlite3_bind_text(st, 2, build_id, -1, SQLITE_STATIC);
  }

  if (!IS_SQL_ERROR(rc)) {
    rc = execute_sql(st);
  }

  if (!IS_SQL_ERROR(rc)) {
    check_name = sqlite3_column_text(st, 1);
  }

  if (IS_SQL_ERROR(rc)) {
    printf("%s(%d)\n", sqlite3_errmsg(db), sqlite3_errcode(db));
  }

  if (check_name) {
    char check_property_value[PROP_VALUE_MAX];

    __system_property_get(check_name, check_property_value);

    printf("%s (%s %s) is not supported.\n", device, build_id, check_property_value);
  }
  else {
    printf("%s (%s) is not supported.\n", device, build_id);
  }

  sqlite3_finalize(st);
}
