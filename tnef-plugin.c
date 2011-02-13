/* Copyright (C) 2005 Novell Inc. */
/* This file is licensed under the GNU GPL v2 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* We include gi18n-lib.h so that we have strings translated directly for this package */
#include <glib/gi18n-lib.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>

#include <camel/camel.h>
// Changed in Evoluiton 2.32:
//#include <camel/camel-mime-part.h>
//#include <camel/camel-folder.h>
//#include <camel/camel-exception.h>
//#include <camel/camel-multipart.h>
//#include <camel/camel-stream-fs.h>

#include <em-format/em-format.h>
#include <mail/em-format-hook.h>
#include <mail/em-utils.h>
// Changed in Evoluiton 2.32:
//#include <e-util/e-error.h>
#include <e-util/e-mktemp.h>

#include "prototypes.h"

void org_gnome_assen_format_tnef(void *ep, EMFormatHookTarget *t){
	char *tmpdir = NULL, *name = NULL, *cmd;
	CamelStream *out;
	struct dirent *d;
	DIR *dir;
	CamelMultipart *mp;
	CamelMimePart *mainpart;
	CamelDataWrapper *content;
	int len, assen_int=0;
	GError *err = NULL;

	tmpdir = e_mkdtemp("tnef-attachment-XXXXXX");
	if (tmpdir == NULL)
		return;

	name = g_build_filename(tmpdir, ".evo-attachment.tnef", NULL);

// Changed in Evoluiton 2.32:
//	out = camel_stream_fs_new_with_name(name, O_RDWR|O_CREAT, 0666);
	out = camel_stream_fs_new_with_name(name, O_RDWR|O_CREAT, 0666, &err);
	if (out == NULL)
		goto fail;
// Changed in Evoluiton 2.32:
//	content = camel_medium_get_content_object((CamelMedium *)t->part);
	content = camel_medium_get_content((CamelMedium *)t->part);
	if (content == NULL)
		goto fail;

// Changed in Evoluiton 2.32:
// 	if (camel_data_wrapper_decode_to_stream(content, out) == -1 || camel_stream_close(out) == -1) {
	if (camel_data_wrapper_decode_to_stream(content, out, &err) == -1 || camel_stream_close(out, &err) == -1) {
// Changed in Evoluiton 2.32:
//		camel_object_unref(out);
		g_object_unref(out);
		goto fail;
	}

// Changed in Evoluiton 2.32:
//	camel_object_unref(out);
	g_object_unref(out);

// 	cmd = g_strdup_printf("ytnef -F -f %s %s", tmpdir, name);
	cmd = g_strdup_printf("ytnef -f %s %s", tmpdir, name);
	system(cmd);
	g_free(cmd);

	dir = opendir(tmpdir);
	if (dir == NULL)
		goto fail;

	mainpart = camel_mime_part_new();

	mp = camel_multipart_new();
	camel_data_wrapper_set_mime_type((CamelDataWrapper *)mp, "multipart/mixed");
	camel_multipart_set_boundary(mp, NULL);

// Changed in Evoluiton 2.32:
//	camel_medium_set_content_object((CamelMedium *)mainpart, (CamelDataWrapper *)mp);
	camel_medium_set_content((CamelMedium *)mainpart, (CamelDataWrapper *)mp);

	while ((d = readdir(dir))) {
		CamelMimePart *part;
		CamelDataWrapper *content;
		CamelStream *stream;
		char *path;
		const char *type;

		if (!strcmp(d->d_name, ".") || !strcmp(d->d_name, "..") || !strcmp(d->d_name, ".evo-attachment.tnef"))
			continue;

		path = g_build_filename(tmpdir, d->d_name, NULL);

// Changed in Evoluiton 2.32:
// 		stream = camel_stream_fs_new_with_name(path, O_RDONLY, 0);
		stream = camel_stream_fs_new_with_name(path, O_RDONLY, 0, &err);
		content = camel_data_wrapper_new();
// Changed in Evoluiton 2.32:
// 		camel_data_wrapper_construct_from_stream(content, stream);
		camel_data_wrapper_construct_from_stream(content, stream, &err);
// Changed in Evoluiton 2.32:
//		camel_object_unref(stream);
		g_object_unref(stream);

		part = camel_mime_part_new();
		camel_mime_part_set_encoding(part, CAMEL_TRANSFER_ENCODING_BINARY);

// Changed in Evoluiton 2.32:
//		camel_medium_set_content_object((CamelMedium *)part, content);
		camel_medium_set_content((CamelMedium *)part, content);
// Changed in Evoluiton 2.32:
//		camel_object_unref(content);
		g_object_unref(content);

		// Changed in Evoluiton 2.30:
		//type = em_utils_snoop_type(part);
		type = em_format_snoop_type(part);
		if (type)
			camel_data_wrapper_set_mime_type((CamelDataWrapper *)part, type);
		
		camel_mime_part_set_filename(part, d->d_name);

		g_free(path);

		camel_multipart_add_part(mp, part);
	}

	closedir(dir);

	len = t->format->part_id->len;
	g_string_append_printf(t->format->part_id, ".tnef");

	if (camel_multipart_get_number(mp) > 0)
		em_format_part_as(t->format, t->stream, mainpart, "multipart/mixed");
	else if (t->item->handler.old)
// Changed in Evoluiton 2.32:
//		t->item->handler.old->handler(t->format, t->stream, t->part, t->item->handler.old);
		t->item->handler.old->handler(t->format, t->stream, t->part, t->item->handler.old, assen_int);

	g_string_truncate(t->format->part_id, len);

// Changed in Evoluiton 2.32:
//	camel_object_unref(mainpart);
	g_object_unref(mainpart);

	goto ok;
fail:
	if (t->item->handler.old)
// Changed in Evoluiton 2.32:
//		t->item->handler.old->handler(t->format, t->stream, t->part, t->item->handler.old);
		t->item->handler.old->handler(t->format, t->stream, t->part, t->item->handler.old, assen_int);
ok:
	g_free(name);
	g_free(tmpdir);
}

int e_plugin_lib_enable(EPlugin *ep, int enable){
	int ok = 0;

	if (enable) {
		int res;

		res = system("ytnef -h > /dev/null");
		/* on F12, a missing command returns "127" */
		if (WEXITSTATUS(res) == 127) {
			g_warning("Cannot find ytnef, tnef decoding is disabled");
			ok = -1;
		}

		bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	} 

	return ok;
}
