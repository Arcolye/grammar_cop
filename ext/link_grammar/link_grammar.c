#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>

#include "link_grammar.h"

static VALUE rlink_linkage_make_cnode_array( CNode * );

void Init_link_grammar() {
	VALUE lg = rb_define_module("LinkGrammar");
	rb_define_module_function(lg, "create_dictionary", create_dictionary, 1);
	rb_define_module_function(lg, "delete_dictionary", delete_dictionary, 1);
	rb_define_module_function(lg, "dictionary_path_set", dictionary_path_set, 1);
	rb_define_module_function(lg, "create_parse_options", create_parse_options, 0);
	rb_define_module_function(lg, "delete_parse_options", delete_parse_options, 1);
	rb_define_module_function(lg, "linkage_limit_set", linkage_limit_set, 2);
	rb_define_module_function(lg, "linkage_limit_get", linkage_limit_get, 1);
	rb_define_module_function(lg, "short_length_set", short_length_set, 2);
	rb_define_module_function(lg, "short_length_get", short_length_get, 1);
	rb_define_module_function(lg, "disjunct_cost_set", disjunct_cost_set, 2);
	rb_define_module_function(lg, "disjunct_cost_get", disjunct_cost_get, 1);
	rb_define_module_function(lg, "min_null_count_set", min_null_count_set, 2);
	rb_define_module_function(lg, "min_null_count_get", min_null_count_get, 1);
	rb_define_module_function(lg, "max_null_count_set", max_null_count_set, 2);
	rb_define_module_function(lg, "max_null_count_get", max_null_count_get, 1);	
	rb_define_module_function(lg, "options_null_block_set", options_null_block_set, 2);
	rb_define_module_function(lg, "options_null_block_get", options_null_block_get, 1);
	rb_define_module_function(lg, "islands_ok_set", islands_ok_set, 2);
	rb_define_module_function(lg, "islands_ok_get", islands_ok_get, 1);
	rb_define_module_function(lg, "create_sentence", create_sentence, 2);
	rb_define_module_function(lg, "parse_sentence", parse_sentence, 2);
	rb_define_module_function(lg, "delete_sentence", delete_sentence, 1);
	rb_define_module_function(lg, "create_linkage", create_linkage, 3);
	rb_define_module_function(lg, "delete_linkage", delete_linkage, 1);
	rb_define_module_function(lg, "print_linkage_diagram", print_linkage_diagram, 1);
	rb_define_module_function(lg, "rlink_linkage_constituent_tree", rlink_linkage_constituent_tree, 1);
  rb_define_module_function(lg, "linkage_count_sublinkages", linkage_count_sublinkages, 1);
	rb_define_module_function(lg, "linkage_count_num_words", linkage_count_num_words, 1);
	rb_define_module_function(lg, "linkage_count_num_links", linkage_count_num_links, 1);	
  
  /* Struct that contains links of a constituent tree (:label, :children, :start, :end) */
  rb_define_const(lg, "CTree", rlink_sLinkageCTree );
  rlink_sLinkageCTree = rb_struct_define( "LinkParserLinkageCTree",
    "label", "children", "start", "end", NULL );
}

//functions for retrieving pointers from ruby VALUE wrapper
DictionaryPtr *retrieve_dictionary(VALUE dict) {
	DictionaryPtr *ptr;
	Data_Get_Struct(dict, DictionaryPtr, ptr);
	return ptr;
}

ParseOptionsPtr *retrieve_parse_options(VALUE opts) {
	ParseOptionsPtr *ptr;
	Data_Get_Struct(opts, ParseOptionsPtr, ptr);
	return ptr;
}

SentencePtr *retrieve_sentence(VALUE sentence) {
	SentencePtr *ptr;
	Data_Get_Struct(sentence, SentencePtr, ptr);
	return ptr;
}

LinkagePtr *retrieve_linkage(VALUE link) {
	LinkagePtr *ptr;
	Data_Get_Struct(link, LinkagePtr, ptr);
	return ptr;
}

//custom routines for freeing memory pointers
void free_dictionary_ptr(DictionaryPtr *ptr) {
	dictionary_delete(ptr->dict);
	free(ptr);
}

void free_parse_options_ptr(ParseOptionsPtr *ptr) {
	parse_options_delete(ptr->opts);
	free(ptr);
}

void free_sentence_ptr(SentencePtr *ptr) {
	sentence_delete(ptr->sentence);
	free(ptr);
}

void free_linkage_ptr(LinkagePtr *ptr) {
	linkage_delete(ptr->linkage);
	free(ptr);
}

VALUE dictionary_path_set(const VALUE self, VALUE dict_path) {
	char *pathstr = StringValuePtr(dict_path);
	dictionary_set_data_dir(pathstr);
	return Qnil;
}

VALUE create_dictionary(const VALUE self, volatile VALUE lang) {
	Dictionary dict = dictionary_create_lang(StringValuePtr(lang));
	DictionaryPtr *ptr = ALLOC(DictionaryPtr);
	ptr->dict = NULL;
	ptr->dict = dict;
	return Data_Wrap_Struct(self, 0, free_dictionary_ptr, ptr);
}

VALUE delete_dictionary(const VALUE self, VALUE dict) {
	DictionaryPtr *ptr = retrieve_dictionary(dict);
	free_dictionary_ptr(ptr);
	return Qnil;
}

VALUE create_parse_options(const VALUE self) {
	Parse_Options options = parse_options_create();
	ParseOptionsPtr *ptr = ALLOC(ParseOptionsPtr);
	ptr->opts = NULL;
	ptr->opts = options;
	return Data_Wrap_Struct(self, 0, free_parse_options_ptr, ptr);
}

VALUE delete_parse_options(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	//int a = parse_options_delete(ptr->opts);
	free_parse_options_ptr(ptr);
	//return INT2FIX(a);
	return Qnil;
}

VALUE linkage_limit_set(const VALUE self, VALUE opts, volatile VALUE limit) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int l = NUM2INT(limit);
	parse_options_set_linkage_limit(ptr->opts, l);
	return Qnil;
}

VALUE linkage_limit_get(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int l = parse_options_get_linkage_limit(ptr->opts);
	return INT2FIX(l);
}

VALUE short_length_set(const VALUE self, VALUE opts, volatile VALUE length) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int l = NUM2INT(length);
	parse_options_set_short_length(ptr->opts, l);
	return Qnil;
}

VALUE short_length_get(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int l = parse_options_get_short_length(ptr->opts);
	return INT2FIX(l);
}

VALUE disjunct_cost_set(const VALUE self, VALUE opts, volatile VALUE cost) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int c = NUM2INT(cost);
	parse_options_set_disjunct_cost(ptr->opts, c);
	return Qnil;
}

VALUE disjunct_cost_get(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int c = parse_options_get_disjunct_cost(ptr->opts);
	return INT2FIX(c);
}

VALUE min_null_count_set(const VALUE self, VALUE opts, volatile VALUE count) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int c = NUM2INT(count);
	parse_options_set_min_null_count(ptr->opts, c);
	return Qnil;
}

VALUE min_null_count_get(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int c = parse_options_get_min_null_count(ptr->opts);
	return INT2FIX(c);
}

VALUE max_null_count_set(const VALUE self, VALUE opts, volatile VALUE count) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int c = NUM2INT(count);
	parse_options_set_max_null_count(ptr->opts, c);
	return Qnil;
}

VALUE max_null_count_get(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int c = parse_options_get_max_null_count(ptr->opts);
	return INT2FIX(c);
}

VALUE options_null_block_set(const VALUE self, VALUE opts, volatile VALUE n_block) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int b = NUM2INT(n_block);
	parse_options_set_null_block(ptr->opts, b);
	return Qnil;
}

VALUE options_null_block_get(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int n_block = parse_options_get_null_block(ptr->opts);
	return INT2FIX(n_block);
}

VALUE islands_ok_set(const VALUE self, VALUE opts, volatile VALUE islands_ok) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int i = NUM2INT(islands_ok);
	parse_options_set_islands_ok(ptr->opts, i);
	return Qnil;
}

VALUE islands_ok_get(const VALUE self, VALUE opts) {
	ParseOptionsPtr *ptr = retrieve_parse_options(opts);
	int i = parse_options_get_islands_ok(ptr->opts);
	return INT2FIX(i);
}

VALUE create_sentence(const VALUE self, volatile VALUE str, VALUE dict) {
	char *text = StringValuePtr(str);
	DictionaryPtr *dict_ptr = retrieve_dictionary(dict);
	Sentence sentence = sentence_create(text, dict_ptr->dict);
	SentencePtr *sent_ptr = ALLOC(SentencePtr);
	sent_ptr->sentence = NULL;
	sent_ptr->sentence = sentence;
	return Data_Wrap_Struct(self, 0, free, sent_ptr);
}

VALUE parse_sentence(const VALUE self, VALUE sentence, VALUE opts) {
	SentencePtr *sent_ptr = retrieve_sentence(sentence);
	ParseOptionsPtr *opts_ptr = retrieve_parse_options(opts);
	int num_linkages = sentence_parse(sent_ptr->sentence, opts_ptr->opts);
	return INT2FIX(num_linkages);
}

VALUE delete_sentence(const VALUE self, VALUE sentence) {
	SentencePtr *ptr = retrieve_sentence(sentence);
	sentence_delete(ptr->sentence);
	//free_sentence_ptr(ptr);
	return Qnil;
}

VALUE create_linkage(const VALUE self, volatile VALUE k, VALUE sentence, VALUE opts) {
	int ck = NUM2INT(k);
	SentencePtr *sent_ptr = retrieve_sentence(sentence);
	ParseOptionsPtr *opts_ptr = retrieve_parse_options(opts);
	Linkage linkage = linkage_create(ck, sent_ptr->sentence, opts_ptr->opts);
	LinkagePtr *link_ptr = ALLOC(LinkagePtr);
	link_ptr->linkage = NULL;
	link_ptr->linkage = linkage;
	return Data_Wrap_Struct(self, 0, free_linkage_ptr, link_ptr);
}

VALUE delete_linkage(const VALUE self, VALUE link) {
	LinkagePtr *link_ptr = retrieve_linkage(link);
	free_linkage_ptr(link_ptr);
	return Qnil;
}

VALUE print_linkage_diagram(const VALUE self, VALUE link) {
	LinkagePtr *link_ptr = retrieve_linkage(link);
	char *diagram = linkage_print_diagram(link_ptr->linkage);
	return rb_str_new2(diagram);
}


VALUE rlink_linkage_make_cnode_array( CNode *ctree ) {
  VALUE nodes = rb_ary_new();
  VALUE rnode;
  CNode *cnode = ctree;
  
  /*
  struct CNode_s {
    char * label;
    CNode * child;
    CNode * next;
    int start, end;
  };
*/
  while ( cnode ) {
    rnode = rb_struct_new( rlink_sLinkageCTree,
    rb_str_new2( linkage_constituent_node_get_label(cnode) ),
    Qnil,
    INT2FIX( linkage_constituent_node_get_start(cnode) ),
    INT2FIX( linkage_constituent_node_get_end(cnode) ) /* end */
    );
    
    /* Make a node array for any children */
    rb_struct_aset( rnode, INT2FIX(1),
    rlink_linkage_make_cnode_array(linkage_constituent_node_get_child(cnode)) );
    
    rb_ary_push( nodes, rnode );
    cnode = linkage_constituent_node_get_next( cnode );
  }
  
  return nodes;
}

VALUE rlink_linkage_constituent_tree(const VALUE self, VALUE link ) {
  LinkagePtr *link_ptr = retrieve_linkage(link);
  CNode *ctree = NULL;
  VALUE rval = Qnil;
  
  ctree = linkage_constituent_tree( link_ptr->linkage );
  rval = rlink_linkage_make_cnode_array( ctree );
  
  linkage_free_constituent_tree( ctree );
  return rval;
}

VALUE linkage_count_sublinkages(const VALUE self, VALUE link) {
	LinkagePtr *link_ptr = retrieve_linkage(link);
	int cnt = linkage_get_num_sublinkages(link_ptr->linkage);
	return INT2FIX(cnt);
}

VALUE linkage_count_num_words(const VALUE self, VALUE link) {
	LinkagePtr *link_ptr = retrieve_linkage(link);
	int cnt = linkage_get_num_words(link_ptr->linkage);
	return INT2FIX(cnt);
}

VALUE linkage_count_num_links(const VALUE self, VALUE link) {
	LinkagePtr *link_ptr = retrieve_linkage(link);
	int cnt = linkage_get_num_links(link_ptr->linkage);
	return INT2FIX(cnt);
}