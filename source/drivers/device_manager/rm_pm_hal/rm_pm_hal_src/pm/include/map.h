/* CC0 (Public domain) - see licenses/CC0 file for details */
#ifndef MAP_H
#define MAP_H

#include <tcon.h>
#include <typesafe_cb.h>
#include <types/sbool.h>
#include <types.h>
#include <types/short_types.h>
#include <string.h>

/**
 * struct map - representation of a map
 *
 * It's exposed here to allow you to embed it and so we can inline the
 * trivial functions.
 */
struct map {
	union {
		struct map_node *n;
		const void	*s;
	} u;
	struct map_node *v;
};

struct map_node {
	/* These point to values or nodes. */
	struct map	child[2];
	/* The byte number where first bit differs. */
	size_t		byte_num;
	/* The bit where these children differ. */
	u8		bit_num;
};

/**
 * strmap_get - get a value from a string map
 * @map: the typed map to search.
 * @member: the string to search for.
 *
 * Returns the node, or NULL if it isn't in the map (and sets errno = ENOENT).
 *
 * Example:
 *	struct map_node *node = strmap_get(&map, "hello");
 */
struct map_node *strmap_get(const struct map *mapp, const char *member);

/**
 * u32map_get - get a value from a u32 map
 * @map: the typed map to search.
 * @member: the u32 to search for.
 *
 * Returns the node, or NULL if it isn't in the map (and sets errno = ENOENT).
 *
 * Example:
 *	struct map_node *node = u32map_get(&map, 32425);
 */
struct map_node *u32map_get(const struct map *mapp, u32 member);

struct map *map_add(struct map *mapp, const void *member, const u8 *bytes, size_t len, struct map_node *newn, const u8 *(*get_bytes)(struct map *));

/**
 * strmap_add - place a member in the string map.
 * @map: the typed map to add to.
 * @member: the string to place in the map.
 * @newn: the allocated node.
 *
 * This returns SFALSE if that string already appears in the map (EEXIST).
 *
 * Note that the pointer is placed in the map, the string is not copied.  If
 * you want a copy in the map, use strdup().  Similarly for the value.
 */
const u8 *str_get_bytes(struct map *mapp);

static inline struct map *strmap_add(struct map *mapp, const char *member,
				     struct map_node *newn)
{
	return map_add(mapp, member, (const u8 *) member, strlen(member), newn,
		       str_get_bytes);
}

/**
 * u32map_add - place a member in the e21 map.
 * @map: the typed map to add to.
 * @member: the u32 to place in the map.
 * @newn: the allocated node.
 *
 * This returns SFALSE if that u32 already appears in the map (EEXIST).
 */
const u8 *u32_get_bytes(struct map *mapp);

static inline struct map *u32map_add(struct map *mapp, u32 member,
				     struct map_node *newn)
{
	return map_add(mapp, (const void *) member, (const u8 *) &member,
		       sizeof(u32), newn, u32_get_bytes);
}

/**
 * strmap_iterate - ordered iteration over a map
 * @map: the typed map to iterate through.
 * @handle: the function to call.
 * @arg: the argument for the function (types should match).
 *
 * @handle's prototype should be:
 *	sbool @handle(const char *member, struct map_node *n, typeof(arg) arg)
 *
 * If @handle returns SFALSE, the iteration will stop.
 * You should not alter the map within the @handle function!
 *
 * Example:
 *	static sbool dump_some(const char *member, struct map_node *n, int *num)
 *	{
 *		// Only dump out num nodes.
 *		if (*(num--) == 0)
 *			return SFALSE;
 *		printf("%s=>%p\n", member, n);
 *		return STRUE;
 *	}
 *
 *	static void dump_map(const struct strmap_intp *mapp)
 *	{
 *		s32 max = 100;
 *		strmap_iterate(mapp, dump_some, &max);
 *		if (max < 0)
 *			printf("... (truncated to 100 entries)\n");
 *	}
 */
#define strmap_iterate(mapp, handle, arg)				\
	strmap_iterate_((mapp),						\
			typesafe_cb_cast(sbool (*)(const char *, struct map_node *, void *),	 \
					 sbool (*)(const char *, struct        map_node *, typeof(arg)), (handle)), (arg))
void strmap_iterate_(const struct map *mapp, sbool (*handle)(const char *, struct map_node *, void *), const void *data);

/**
 * u32map_iterate - ordered iteration over a map
 * @map: the typed map to iterate through.
 * @handle: the function to call.
 * @arg: the argument for the function (types should match).
 *
 * @handle's prototype should be:
 *	sbool @handle(u32 member, struct map_node *n, typeof(arg) arg)
 *
 * If @handle returns SFALSE, the iteration will stop.
 * You should not alter the map within the @handle function!
 */
#define u32map_iterate(mapp, handle, arg)				\
	u32map_iterate_((mapp),						\
			typesafe_cb_cast(sbool (*)(u32, struct map_node *, void *),	\
					 sbool (*)(u32, struct map_node *, typeof(arg)), (handle)), (arg))
void u32map_iterate_(const struct map *mapp, sbool (*handle)(u32, struct map_node *, void *), const void *data);
/**
 * strmap_prefix - return a submap matching a prefix
 * @map: the map.
 * @prefix: the prefix.
 *
 * This returns a pointer into @map, so don't alter @map while using
 * the return value.  You can use strmap_iterate() or strmap_get()
 * on the returned pointer.
 *
 * Example:
 *	static void dump_prefix(const struct map *mapp,
 *				const char *prefix)
 *	{
 *		s32 max = 100;
 *		printf("Nodes with prefix %s:\n", prefix);
 *		strmap_iterate(strmap_prefix(mapp, prefix), dump_some, &max);
 *		if (max < 0)
 *			printf("... (truncated to 100 entries)\n");
 *	}
 */
const struct map *strmap_prefix(const struct map *mapp, const char *prefix);

#endif /* _STRMAP_H_ */
