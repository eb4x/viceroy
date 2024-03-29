#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "savegame.h"

void print_head(  const struct savegame::head   *head);
void print_player(const struct savegame::player *player,                        int just_this_one = -1);
void print_other( const struct savegame::other  *other);
void print_colony(const struct savegame *sg, const struct savegame::colony *colony, uint16_t colony_count, int just_this_one = -1);
void print_unit(  const struct savegame::unit   *unit,   uint16_t unit_count,   int just_this_one = -1);
void print_nation(const struct savegame::nation *nation,                        int just_this_one = -1);
void print_tribe( const struct savegame::tribe  *tribe,  uint16_t tribe_count,  int just_this_one = -1);
void print_indian(const struct savegame::indian_relations *ir,                  int just_this_one = -1);
void print_stuff( const struct savegame::stuff  *stuff);
void print_map(   const struct savegame::map    *map);
void print_tail(  const struct savegame::tail   *tail);
void print_route(const struct savegame *sg, const struct savegame::trade_route *route, int just_this_one = -1);

void dump(void *address, size_t bytes, const char *filename);

void print_help(const char *prog){
	fprintf(stderr, "Usage: %s [options] <COLONY0*.SAV> ...\n", prog);
	fprintf(stderr, "OPTIONs:\n");
	fprintf(stderr, "-h, --help       displays this help message          \n");
	fprintf(stderr, "                                                     \n");
	fprintf(stderr, "-H, --head       displays head section of savegame   \n");
	fprintf(stderr, "-T, --tail       displays tail section of savegame   \n");
	fprintf(stderr, "-o, --other      displays other section of savegame  \n");
	fprintf(stderr, "-s, --stuff      displays stuff section of savegame  \n");
	fprintf(stderr, "-m, --map        displays map section of savegame    \n");
	fprintf(stderr, "                                                     \n");
	fprintf(stderr, "If N is given, displays a single entry in the section\n");
	fprintf(stderr, "-pN, --player=N  displays player section of savegame \n");
	fprintf(stderr, "-uN, --unit=N    displays unit section of savegame   \n");
	fprintf(stderr, "-nN, --nation=N  displays nation section of savegame \n");
	fprintf(stderr, "-tN, --tribe=N   displays tribe section of savegame  \n");
	fprintf(stderr, "-iN, --indian=N  displays indian section of savegame \n");
	fprintf(stderr, "-rN, --route=N   displays trade route section        \n");
	fprintf(stderr, "                                                     \n");
	fprintf(stderr, "--colony10  writes modificaions to COLONY10.SAV      \n");
}

int main(int argc, char *argv[])
{
	assert(sizeof (struct savegame::head)   == 158);
	assert(sizeof (struct savegame::player) ==  52);
	assert(sizeof (struct savegame::colony) == 202);
	assert(sizeof (struct savegame::unit)   ==  28);
	assert(sizeof (struct savegame::nation) == 316);
	assert(sizeof (struct savegame::tribe)  ==  18);
	assert(sizeof (struct savegame::stuff)  == 727);
	assert(sizeof (struct savegame::map)    == 58*72*4);
	assert(sizeof (struct savegame::trade_route) == 74);

	int c, optindex = 0;

	/* Flags
	 *  -1 print all
	 *  0 don't print any
	 *  n print specific entry
	 */
	int opt_head = 0, opt_player = 0, opt_other = 0, opt_colony = 0, opt_unit = 0,
	    opt_nation = 0, opt_tribe = 0, opt_stuff = 0, opt_indian = 0, opt_map = 0,
	    opt_tail = 0, opt_route = 0, opt_help = 0, opt_colony10 = 0;

	static struct option long_options[] = {
		{ "head",     no_argument,       NULL,          'H' },
		{ "player",   optional_argument, NULL,          'p' },
		{ "other",    no_argument,       NULL,          'o' },
		{ "colony",   optional_argument, NULL,          'c' },
		{ "unit",     optional_argument, NULL,          'u' },
		{ "nation",   optional_argument, NULL,          'n' },
		{ "tribe",    optional_argument, NULL,          't' },
		{ "indian",   optional_argument, NULL,          'i' },
		{ "stuff",    no_argument,       NULL,          's' },
		{ "map",      no_argument,       NULL,          'm' },
		{ "tail",     no_argument,       NULL,          'T' },
		{ "route",    optional_argument, NULL,          'r' },
		{ "colony10", no_argument,       &opt_colony10, -1  },
		{ "help",     no_argument,       NULL,          'h' },
		{ NULL,       no_argument, NULL,  0  }
	};

	while ((c = getopt_long(argc, argv, ":Hp::oc::u::n::t::i::r::smTh", long_options, &optindex)) != -1) {
		switch (c) {

			case 0:
				/* If this option set a flag, do nothing else now. */
				if (long_options[optindex].flag != 0)
					break;
				printf("option %s", long_options[optindex].name);
				if (optarg)
					printf(" with arg %s", optarg);
				printf("\n");

			case 'H': opt_head   = -1; break;
			case 'p': opt_player = -1;
				if (optarg && isdigit(optarg[0]) )
					opt_player = atoi(optarg) + 1;
				break;
			case 'o': opt_other  = -1; break;
			case 'c': opt_colony = -1;
				if (optarg && isdigit(optarg[0]) )
					opt_colony = atoi(optarg) + 1;
				break;
			case 'u': opt_unit   = -1;
				if (optarg && isdigit(optarg[0]) )
					opt_unit = atoi(optarg) + 1;
				break;
			case 'n': opt_nation = -1;
				if (optarg && isdigit(optarg[0]) )
					opt_nation = atoi(optarg) + 1;
				break;
			case 't': opt_tribe  = -1;
				if (optarg && isdigit(optarg[0]) )
					opt_tribe = atoi(optarg) + 1;
				break;
			case 'i': opt_indian = -1;
				if (optarg && isdigit(optarg[0]) )
					opt_indian = atoi(optarg) + 1;
				break;
			case 'r': opt_route = -1;
				if (optarg && isdigit(optarg[0]) )
					opt_route = atoi(optarg) + 1;
				break;
			case 's': opt_stuff  = -1; break;
			case 'm': opt_map    = -1; break;
			case 'T': opt_tail   = -1; break;

			case '?': /* fall through to 'h'*/
				fprintf(stderr, "Unknown option '%s'\n", argv[optind-1]);
			case 'h':
				print_help(argv[0]);
				exit(EXIT_FAILURE);

			default:
				fprintf(stderr, "case '%c' (0x%02x): ", c, c);
				if (optopt) fprintf(stderr, "optopt: %d ", optopt);
				if (optarg) fprintf(stderr, "optarg: %s ", optarg);
				fprintf(stderr, "\n");
				break;
		}
	}

	if (optind >= argc) {
		print_help(argv[0]);
		exit(EXIT_FAILURE);
	}

	for (int fi = optind; fi < argc; ++fi) {
		struct savegame sg;

		FILE *fp = fopen(argv[fi], "r");	

		if (fp == NULL) {
			printf("Could not open file: %s\n", argv[fi]);
			exit(EXIT_FAILURE);
		}
	
		size_t res = 0;
		res = fread(&sg.head, sizeof (struct savegame::head), 1, fp);
		res = fread(&sg.player, sizeof (struct savegame::player), 4, fp);
		res = fread(&sg.other, sizeof (struct savegame::other), 1, fp);

		sg.colony = (struct savegame::colony *) malloc(sizeof (struct savegame::colony) * sg.head.colony_count);
		res = fread(sg.colony, sizeof (struct savegame::colony), sg.head.colony_count, fp);
	
		sg.unit   = (struct savegame::unit *)   malloc(sizeof (struct savegame::unit)   * sg.head.unit_count);
		res = fread(sg.unit, sizeof (struct savegame::unit), sg.head.unit_count, fp);
	
		res = fread(sg.nation, sizeof (struct savegame::nation), 4, fp);
	
		sg.tribe  = (struct savegame::tribe *)  malloc(sizeof (struct savegame::tribe)  * sg.head.tribe_count);
		res = fread(sg.tribe, sizeof (struct savegame::tribe), sg.head.tribe_count, fp);
	
		res = fread(&sg.indian_relations, sizeof (struct savegame::indian_relations), 8, fp);
		res = fread(&sg.stuff, sizeof (struct savegame::stuff), 1, fp);
		res = fread(&sg.map, sizeof (struct savegame::map), 1, fp);
		res = fread(&sg.tail, sizeof (struct savegame::tail), 1, fp);
		res = fread(&sg.trade_route, sizeof (struct savegame::trade_route), 12, fp);
	
		fclose(fp);
	
		if (opt_head)
			print_head(&(sg.head));
	
		if (opt_player)
			print_player(sg.player, (opt_player == -1) ? opt_player: opt_player - 1);
	
		if (opt_other)
			print_other(&(sg.other));
	
		if (opt_colony)
			print_colony(&sg, sg.colony, sg.head.colony_count, (opt_colony == -1) ? opt_colony : opt_colony - 1);
	
		if (opt_unit)
			print_unit(sg.unit, sg.head.unit_count, (opt_unit == -1) ? opt_unit : opt_unit - 1);
	
		if (opt_nation)
			print_nation(sg.nation, (opt_nation == -1) ? opt_nation : opt_nation - 1);
	
		if (opt_tribe)
			print_tribe(sg.tribe, sg.head.tribe_count, (opt_tribe == -1) ? opt_tribe : opt_tribe - 1);
	
		if (opt_indian)
			print_indian(sg.indian_relations, (opt_indian == -1) ? opt_indian : opt_indian - 1);
	
		if (opt_stuff)
			print_stuff(&(sg.stuff));
	
		if (opt_map)
			print_map(&(sg.map));
	
		if (opt_tail)
			print_tail(&(sg.tail));
	
		if (opt_route)
			print_route(&sg, sg.trade_route, (opt_route == -1) ? opt_route : opt_route - 1);

		if (opt_colony10) {

			/* Find our player */
			int player_nation = -1;
			for (int i = 0; i < 4; ++i) {
				if (sg.player[i].control == 0) {
					player_nation = i;
					break;
				}
			}

			sg.nation[player_nation].gold = 4000000;

			for (int i = 0; i < sg.head.colony_count; ++i) {
				if (sg.colony[i].nation == player_nation) {

					for (int j = 0; j < 32; ++j) {
						switch (j) {
							case 0:
							case 1:
							case 2:
								sg.colony[i].profession[j] = 0x11; // elder statesman
								sg.colony[i].occupation[j] = 0x11;
								break;
							case 3:
							case 4:
								sg.colony[i].profession[j] = 0x0d; // carpenter
								sg.colony[i].occupation[j] = 0x0d;
								break;
							case 5:
							case 6:
								sg.colony[i].profession[j] = 0x0e; // blacksmith
								sg.colony[i].occupation[j] = 0x0e;
								break;
							case 7:
								sg.colony[i].profession[j] = 0x05; // lumberjack
								sg.colony[i].occupation[j] = 0x05;
								sg.colony[i].tiles[0] = j;
								break;
							case 8:
								sg.colony[i].profession[j] = 0x08; // fisherman
								sg.colony[i].occupation[j] = 0x08;
								sg.colony[i].tiles[1] = j;
								break;
							case 9:
								sg.colony[i].profession[j] = 0x06; // oreminer
								sg.colony[i].occupation[j] = 0x06;
								sg.colony[i].tiles[4] = j;
								break;

							default:
								continue;
						}
					}

					sg.colony[i].population = 10;

					sg.colony[i].buildings.docks = 1;
					sg.colony[i].buildings.custom_house = 1;
					continue;
				}

				// Opposing nations, remove pesky stockades
				sg.colony[i].buildings.stockade = 0;
			}

			FILE *fop = fopen("COLONY10.SAV", "w");
			fwrite(&sg.head, sizeof (struct savegame::head), 1, fop);
			fwrite(&sg.player, sizeof (struct savegame::player), 4, fop);
			fwrite(&sg.other, sizeof (struct savegame::other), 1, fop);
			fwrite(sg.colony, sizeof (struct savegame::colony), sg.head.colony_count, fop);
			fwrite(sg.unit, sizeof (struct savegame::unit), sg.head.unit_count, fop);
			fwrite(sg.nation, sizeof (struct savegame::nation), 4, fop);
			fwrite(sg.tribe, sizeof (struct savegame::tribe), sg.head.tribe_count, fop);
			fwrite(sg.indian_relations, sizeof (struct savegame::indian_relations), 8, fop);
			fwrite(&sg.stuff, sizeof (struct savegame::stuff), 1, fop);
			fwrite(&sg.map, sizeof (struct savegame::map), 1, fop);
			fwrite(&sg.tail, sizeof(struct savegame::tail), 1, fop);
			fwrite(&sg.trade_route, sizeof(struct savegame::trade_route), 12, fop);
			fclose(fop);
		}

		free(sg.colony);
		free(sg.unit);
		free(sg.tribe);
	}
	
	return EXIT_SUCCESS;
}

void print_head(  const struct savegame::head   *head)
{
	printf("-- head --\n");

	printf("Signature: %s: %s\n",
		head->sig_colonize,
		strncmp(head->sig_colonize, "COLONIZE", 9) ? "INVALID" : "OK");

	printf("Map size: %2d x %2d\n",
		head->map_size_x, head->map_size_y);

	printf("Difficulty: %s\n",
		difficulty_list[head->difficulty]);

	printf("%s %4d, Turn: %2d, Tribes: %d, Units: %d, Colonies: %d, Trade Routes: %d\n",
		head->autumn ? "Autumn" : "Spring",
		head->year,
		head->turn,
		head->tribe_count,
		head->unit_count,
		head->colony_count,
		head->trade_route_count);

//	printf("Active unit: "); print_unit(sg.unit, sg.head.unit_count, head->active_unit);

	assert(head->unit_count <= 300);

	for (int i = 0; i < sizeof (head->unk0); ++i)
		printf("%02x ", head->unk0[i]);
	printf("\n\n");

	printf("Colony report options\n");
	printf("%d - Labels on buildings\n",                head->colony_report_options.labels_on_buildings);
	printf("%d - Labels on cargo and terrain\n",        head->colony_report_options.labels_on_cargo_and_terrain);
	printf("%d - report_when_colonists_trained\n",      head->colony_report_options.report_when_colonists_trained);
	printf("%d - report_food_shortages\n",              head->colony_report_options.report_food_shortages);
	printf("%d - report_raw_materials_shortages\n",     head->colony_report_options.report_raw_materials_shortages);
	printf("%d - report_tools_needed_for_production\n", head->colony_report_options.report_tools_needed_for_production);
	printf("%d - report_inefficient_government\n",      head->colony_report_options.report_inefficient_government);
	printf("%d - report_new_cargos_available\n",        head->colony_report_options.report_new_cargos_available);
	printf("%d - report_sons_of_liberty_membership\n",  head->colony_report_options.report_sons_of_liberty_membership);
	printf("%d - report_rebel_majorities\n",            head->colony_report_options.report_rebel_majorities);
	printf("%d - unused\n",                             head->colony_report_options.unused);
	printf("\n");
	assert(head->colony_report_options.unused == 0);

	printf("Tutorial 13: %5s\n", head->tut1.nr13 ? "true" : "false");
	printf("Tutorial 14: %5s\n", head->tut1.nr14 ? "true" : "false");
	printf("tut1.x3    : %5s\n", head->tut1.unk3 ? "true" : "false");
	printf("Tutorial 15: %5s\n", head->tut1.nr15 ? "true" : "false");
	printf("Tutorial 16: %5s\n", head->tut1.nr16 ? "true" : "false");
	printf("Tutorial 17: %5s\n", head->tut1.nr17 ? "true" : "false");
	printf("tut1.x7    : %5s\n", head->tut1.unk7 ? "true" : "false");
	printf("Tutorial 19: %5s\n", head->tut1.nr19 ? "true" : "false");
	printf("\n");

	for (int i = 0; i < sizeof (head->unk1); ++i)
		printf("%02x ", head->unk1[i]);
	printf("\n\n");

	printf("Set Game Options:\n");
	printf("  %c Show Indian Moves\n",   head->game_options.show_indian_moves   ? '*' : ' ');
	printf("  %c Show Foreign Moves\n",  head->game_options.show_foreign_moves  ? '*' : ' ');
	printf("  %c Fast Piece Slide\n",    head->game_options.fast_piece_slide    ? '*' : ' ');
	printf("  %c End of Turn\n",         head->game_options.end_of_turn         ? '*' : ' ');
	printf("  %c Autosave\n",            head->game_options.autosave            ? '*' : ' ');
	printf("  %c Combat Analysis\n",     head->game_options.combat_analysis     ? '*' : ' ');
	printf("  %c Water Color Cycling\n", head->game_options.water_color_cycling ? ' ' : '*'); // I don't know why it's inverted
	printf("  %c Tutorial Hints\n",      head->game_options.tutorial_hints      ? '*' : ' ');

	assert(head->game_options.unknown7 == 0);

	printf("HowToWin        : %5s\n", head->tut2.howtowin ? "true" : "false");

	printf("Set Sound Options:\n");
	printf("  %c Background Music\n", head->tut2.background_music ? '*' : ' ');
	printf("  %c Event Music\n",      head->tut2.event_music      ? '*' : ' ');
	printf("  %c Sound Effects\n",    head->tut2.sound_effects    ? '*' : ' ');

	printf("Tutorial  1: %5s\n", head->tut2.nr1  ? "true" : "false");
	printf("Tutorial  2: %5s\n", head->tut2.nr2  ? "true" : "false");
	printf("Tutorial  3: %5s\n", head->tut2.nr3  ? "true" : "false");
	printf("Tutorial  4: %5s\n", head->tut2.nr4  ? "true" : "false");
	printf("Tutorial  5: %5s\n", head->tut3.nr5  ? "true" : "false");
	printf("Tutorial  6: %5s\n", head->tut3.nr6  ? "true" : "false");
	printf("Tutorial  7: %5s\n", head->tut3.nr7  ? "true" : "false");
	printf("Tutorial  8: %5s\n", head->tut3.nr8  ? "true" : "false");
	printf("Tutorial  9: %5s\n", head->tut3.nr9  ? "true" : "false");
	printf("Tutorial 10: %5s\n", head->tut3.nr10 ? "true" : "false");
	printf("Tutorial 11: %5s\n", head->tut3.nr11 ? "true" : "false");
	printf("Tutorial 12: %5s\n", head->tut3.nr12 ? "true" : "false");
	printf("\n");

	assert(head->tut2.nr2 == 0); // I don't think this is used

	printf("numbers00: %3d(%04x)\n", head->numbers00, head->numbers00);
	printf("numbers01: %3d(%04x)\n", head->numbers01, head->numbers01);

	for (int i = 0; i < 3; ++i)
		printf("numbers02.%d: %3d(%04x)\n", i, head->numbers02[i], head->numbers02[i]);

	for (int i = 0; i < 2; ++i)
		printf("numbers03.%d: %3d(%04x)\n", i, head->numbers03[i], head->numbers03[i]);

	printf("numbers04: %3d(%04x)\n", head->numbers04, head->numbers04);
	printf("\n");

	printf("Founding fathers:\n");
	for (int i = 0; i < 25; ++i) {
		switch (head->founding_father[i]) {
			case -1: printf("( -1)"); break;
			case  0: printf("(Eng)"); break;
			case  1: printf("(Fra)"); break;
			case  2: printf("(Spa)"); break;
			case  3: printf("(Dut)"); break;
			default: printf("(Unk)"); break;
		}
		printf(", %s\n", founding_father_list[i]);
	}
	printf("\n");

	for (int i = 0; i < 3; ++i)
		printf("numbers05.%d: %3d(%04x)\n", i, head->numbers05[i], head->numbers05[i]);
	printf("\n");

	for (int i = 0; i < 4; ++i)
		printf("National relation: %3d %s\n", head->nation_relation[i], nation_list[i]);
	printf("\n");

	for (int i = 0; i < 5; ++i)
		printf("numbers06.%d: %3d(%04x)\n", i, head->numbers06[i], head->numbers06[i]);
	printf("\n");

	for (int i = 0; i < 4; ++i) {
		switch (i) {
			case 0: printf("%d Regulars, ",  head->expeditionary_force[i]); break;
			case 1: printf("%d Cavalry,  ",  head->expeditionary_force[i]); break;
			case 2: printf("%d Man-O-War, ", head->expeditionary_force[i]); break;
			case 3: printf("%d Artillery\n", head->expeditionary_force[i]); break;
		}
	}

	for (int i = 0; i < 4; ++i)
		printf("numbers07.%d: %3d(%04x)\n", i, head->numbers07[i], head->numbers07[i]);
	printf("\n");

	for (int i = 0; i < 16; ++i)
		printf("count down[%2d]: %3d\n", i, head->count_down[i]);
	printf("\n");

	printf("event_discovery_of_the_new_world     : %5s\n", head->event.discovery_of_the_new_world     ? "true" : "false");
	printf("event_building_a_colony              : %5s\n", head->event.building_a_colony              ? "true" : "false");
	printf("event_meeting_the_natives            : %5s\n", head->event.meeting_the_natives            ? "true" : "false");
	printf("event_the_aztec_empire               : %5s\n", head->event.the_aztec_empire               ? "true" : "false");
	printf("event_the_inca_nation                : %5s\n", head->event.the_inca_nation                ? "true" : "false");
	printf("event_discovery_of_the_pacific_ocean : %5s\n", head->event.discovery_of_the_pacific_ocean ? "true" : "false");
	printf("event_entering_indian_village        : %5s\n", head->event.entering_indian_village        ? "true" : "false");
	printf("event_the_fountain_of_youth          : %5s\n", head->event.the_fountain_of_youth          ? "true" : "false");
	printf("event_cargo_from_the_new_world       : %5s\n", head->event.cargo_from_the_new_world       ? "true" : "false");
	printf("event_meeting_fellow_europeans       : %5s\n", head->event.meeting_fellow_europeans       ? "true" : "false");
	printf("event_colony_burning                 : %5s\n", head->event.colony_burning                 ? "true" : "false");
	printf("event_colony_destroyed               : %5s\n", head->event.colony_destroyed               ? "true" : "false");
	printf("event_indian_raid                    : %5s\n", head->event.indian_raid                    ? "true" : "false");
	printf("event_woodcut14                      : %5s\n", head->event.woodcut14                      ? "true" : "false");
	printf("event_woodcut15                      : %5s\n", head->event.woodcut15                      ? "true" : "false");
	printf("event_woodcut16                      : %5s\n", head->event.woodcut16                      ? "true" : "false");

	for (int i = 0; i < 2; ++i)
		printf("%02x ", head->unkb[i]);
	printf("\n");

	printf("\n");
}

void print_player(const struct savegame::player *player, int just_this_one)
{
	printf("-- player --\n");

	int start = (just_this_one == -1) ? 0 : just_this_one;

	for (int i = start; i < 4; ++i) {
		printf("%-11s: %23s / %23s : ", nation_list[i], player[i].name, player[i].country);
		switch (player[i].control) {
			case savegame::player::PLAYER:    printf("Player    "); break;
			case savegame::player::AI:        printf("AI        "); break;
			case savegame::player::WITHDRAWN: printf("Withdrawn "); break;
			default: printf("UNKNOWN (%02x)", player[i].control);
		}
		printf("diplomacy: %02x ", player[i].diplomacy);

		printf("unknown: %02x ", player[i].unk00);
		printf("colonies: %2d\n", player[i].founded_colonies);

		if (just_this_one != -1)
			break;
	}
	printf("\n");
}

void print_other( const struct savegame::other  *other)
{
	printf("-- other --\n");

	for (int i = 0; i < sizeof (other->unkXX_xx); ++i)
		printf("%02x ", other->unkXX_xx[i]);
	printf("\n\n");
}

void print_colony(const struct savegame *sg, const struct savegame::colony *colony, uint16_t colony_count, int just_this_one)
{
	printf("-- colonies --\n");

	/* Find our player */
	int player_nation = -1;
	for (int i = 0; i < 4; ++i) {
		if (sg->player[i].control == 0) {
			player_nation = i;
			break;
		}
	}

	int start = (just_this_one == -1) ? 0 : just_this_one;
	for (int i = start; i < colony_count; ++i) {
		if ( colony[i].nation != player_nation) /* Skip printing colonies not under player control */
			continue;

		printf("[%3d] (%3d, %3d): %2d %s\n", i, colony[i].x, colony[i].y, colony[i].population, colony[i].name);

		for (int j = 0; j < sizeof (colony[i].unk0) ; ++j)
			printf("%02x ", colony[i].unk0[j]);
		printf("\n");

		printf("Colonists;\n");
		for (int j = 0; j < colony[i].population; ++j)
			printf("[%2d]  %s working as %s\n",
				j, profession_list[ colony[i].profession[j] ] ,
				   profession_list[ colony[i].occupation[j] ] );
		printf("\n");

		for (int j = 0; j < sizeof (colony[i].unk6); ++j)
			printf("%02x ", colony[i].unk6[j]);
		printf("\n\n");

		printf("%2d | %2d | %2d\n", colony[i].tiles[4], colony[i].tiles[0], colony[i].tiles[5]);
		printf("-------------\n");
		printf("%2d |    | %2d\n",  colony[i].tiles[3],                     colony[i].tiles[1]);
		printf("-------------\n");
		printf("%2d | %2d | %2d\n", colony[i].tiles[7], colony[i].tiles[2], colony[i].tiles[6]);
		printf("\n");

		for (int j = 0; j < sizeof (colony[i].unk8); ++j)
			printf("%02x ", colony[i].unk8[j]);
		printf("\n");

		printf("Colony buildings:\n");
			switch( colony[i].buildings.stockade ) {
				case 0: break;
				case 1: printf(" stockade\n"); break;
				case 3: printf(" fort    \n"); break;
				case 7: printf(" fortress\n"); break;
				default: printf("a0: %d\n", colony[i].buildings.stockade); break;
			}

			switch ( colony[i].buildings.armory ) {
				case 0: break;
				case 1: printf(" armory  \n"); break;
				case 3: printf(" magazine\n"); break;
				case 7: printf(" arsenal \n"); break;
				default: printf("a1: %d\n", colony[i].buildings.armory); break;
			}

			switch ( colony[i].buildings.docks ) {
				case 0: break;
				case 1: printf(" docks   \n"); break;
				case 3: printf(" dry dock\n"); break;
				case 7: printf(" shipyard\n"); break;
				default: printf("a2: %d\n", colony[i].buildings.docks); break;
			}

			switch ( colony[i].buildings.town_hall ) {
				case 0: break;
				case 1: printf(" town hall\n"); break;
				case 3: printf("a3: 3\n"); break;
				case 7: printf("a3: 7\n"); break;
				default: printf("a3: %d\n", colony[i].buildings.town_hall); break;
			}

			switch ( colony[i].buildings.schoolhouse ) {
				case 0: break;
				case 1: printf(" schoolhouse\n"); break;
				case 3: printf(" college    \n"); break;
				case 7: printf(" university \n"); break;
				default: printf("a4: %d\n", colony[i].buildings.schoolhouse); break;
			}

			switch ( colony[i].buildings.warehouse ) {
				case 0: break;
				case 1: printf("warehouse\n"); break;
				case 3: printf("warehouse (expansion)\n"); break;
				default: printf("a5: %d\n", colony[i].buildings.warehouse); break;
			}

			switch ( colony[i].buildings.stables ) {
				case 0: break;
				case 1: printf("stables\n"); break;
				default: printf("b1: %d\n", colony[i].buildings.stables); break;
			}

			switch ( colony[i].buildings.custom_house ) {
				case 0: break;
				case 1: printf("custom house\n"); break;
				default: printf("b2: %d\n", colony[i].buildings.custom_house); break;
			}

			switch ( colony[i].buildings.printing_press ) {
				case 0: break;
				case 1: printf("printing press\n"); break;
				case 3: printf("newspaper     \n"); break;
				default: printf("b3: %d\n", colony[i].buildings.printing_press); break;
			}

			switch ( colony[i].buildings.weavers_house ) {
				case 0: break;
				case 1: printf("weaver's house\n"); break;
				case 3: printf("weaver's shop \n"); break;
				case 7: printf("textile mill  \n"); break;
				default: printf("b4: %d\n", colony[i].buildings.weavers_house); break;
			}

			switch ( colony[i].buildings.tobacconists_house ) {
				case 0: break;
				case 1: printf("tobacconist's house\n"); break;
				case 3: printf("tobacconist's shop \n"); break;
				case 7: printf("cigar factory      \n"); break;
				default: printf("b5: %d\n", colony[i].buildings.tobacconists_house); break;
			}

			switch ( colony[i].buildings.rum_distillers_house ) {
				case 0: break;
				case 1: printf("rum distiller's house\n"); break;
				case 3: printf("rum distillery\n"); break;
				case 7: printf("rum factory\n"); break;
				default: printf("b6: %d\n", colony[i].buildings.rum_distillers_house); break;
			}

			switch ( colony[i].buildings.capitol ) {
				case 0: break;
				case 1: printf("capitol\n"); break;
				case 3: printf("capitol (expansion)\n"); break;
				default: printf("b7: %d\n", colony[i].buildings.capitol); break;
			}

			switch ( colony[i].buildings.fur_traders_house ) {
				case 0: break;
				case 1: printf("fur trader's house\n"); break;
				case 3: printf("fur trading post  \n"); break;
				case 7: printf("fur factory       \n"); break;
				default: printf("c0: %d\n", colony[i].buildings.fur_traders_house);
			}

			switch ( colony[i].buildings.carpenters_shop ) {
				case 0: break;
				case 1: printf("carpenter's shop\n"); break;
				case 3: printf("lumber mill\n"); break;
				default: printf("c1: %d\n", colony[i].buildings.carpenters_shop);
			}

			switch ( colony[i].buildings.church ) {
				case 0: break;
				case 1: printf("church\n"); break;
				case 3: printf("cathedral: 3\n"); break;
				default: printf("c2: %d\n", colony[i].buildings.church);
			}

			switch ( colony[i].buildings.blacksmiths_house ) {
				case 0: break;
				case 1: printf("blacksmith's house\n"); break;
				case 3: printf("blacksmith's shop \n"); break;
				case 7: printf("iron works        \n"); break;
				default: printf("c3: %d\n", colony[i].buildings.blacksmiths_house);
			}

			assert(colony[i].buildings.unused == 0);

		printf("Custom house:\n");
		printf("  %c food       \n", colony[i].custom_house.food        ? '*' : ' ');
		printf("  %c sugar      \n", colony[i].custom_house.sugar       ? '*' : ' ');
		printf("  %c tobacco    \n", colony[i].custom_house.tobacco     ? '*' : ' ');
		printf("  %c cotton     \n", colony[i].custom_house.cotton      ? '*' : ' ');
		printf("  %c furs       \n", colony[i].custom_house.furs        ? '*' : ' ');
		printf("  %c lumber     \n", colony[i].custom_house.lumber      ? '*' : ' ');
		printf("  %c ore        \n", colony[i].custom_house.ore         ? '*' : ' ');
		printf("  %c silver     \n", colony[i].custom_house.silver      ? '*' : ' ');
		printf("  %c horses     \n", colony[i].custom_house.horses      ? '*' : ' ');
		printf("  %c rum        \n", colony[i].custom_house.rum         ? '*' : ' ');
		printf("  %c cigars     \n", colony[i].custom_house.cigars      ? '*' : ' ');
		printf("  %c cloth      \n", colony[i].custom_house.cloth       ? '*' : ' ');
		printf("  %c coats      \n", colony[i].custom_house.coats       ? '*' : ' ');
		printf("  %c trade_goods\n", colony[i].custom_house.trade_goods ? '*' : ' ');
		printf("  %c tools      \n", colony[i].custom_house.tools       ? '*' : ' ');
		printf("  %c muskets    \n", colony[i].custom_house.muskets     ? '*' : ' ');
		printf("\n");

		for (int j = 0; j < sizeof (colony[i].unka); ++j)
			printf("%02x ", colony[i].unka[j]);
		printf("\n");

		printf("%3d hammers ", colony[i].hammers);
		printf("producing: ");
		switch (colony[i].building_in_production) {
			case 255: printf("Nothing              \n"); break;
			case   0: printf("Stockade             \n"); break;
			case   1: printf("Fort                 \n"); break;
			case   2: printf("Fortress             \n"); break;
			case   3: printf("Armory               \n"); break;
			case   4: printf("Magazine             \n"); break;
			case   5: printf("Arsenal              \n"); break;
			case   6: printf("Docks                \n"); break;
			case   7: printf("Drydock              \n"); break;
			case   8: printf("Shipyard             \n"); break;
			case   9: printf("Town Hall            \n"); break;
			case  10: printf("Town Hall            \n"); break;
			case  11: printf("Town Hall            \n"); break;
			case  12: printf("Schoolhouse          \n"); break;
			case  13: printf("College              \n"); break;
			case  14: printf("University           \n"); break;
			case  15: printf("Warehouse            \n"); break;
			case  16: printf("Warehouse Expansion  \n"); break;
			case  17: printf("Stable               \n"); break;
			case  18: printf("Custom House         \n"); break;
			case  19: printf("Printing Press       \n"); break;
			case  20: printf("Newspaper            \n"); break;
			case  21: printf("Weaver's House       \n"); break;
			case  22: printf("Weaver's Shop        \n"); break;
			case  23: printf("Textile Mill         \n"); break;
			case  24: printf("Tobacconist's House  \n"); break;
			case  25: printf("Tobacconist's Shop   \n"); break;
			case  26: printf("Cigar Factory        \n"); break;
			case  27: printf("Rum Distiller's House\n"); break;
			case  28: printf("Rum Distillery       \n"); break;
			case  29: printf("Rum Factory          \n"); break;
			case  30: printf("Capitol              \n"); break;
			case  31: printf("Capitol Expansion    \n"); break;
			case  32: printf("Fur Trader's House   \n"); break;
			case  33: printf("Fur Trading Post     \n"); break;
			case  34: printf("Fur Factory          \n"); break;
			case  35: printf("Carpenter's Shop     \n"); break;
			case  36: printf("Lumber Mill          \n"); break;
			case  37: printf("Church               \n"); break;
			case  38: printf("Cathedral            \n"); break;
			case  39: printf("Blacksmith's House   \n"); break;
			case  40: printf("Blacksmith's Shop    \n"); break;
			case  41: printf("Iron Works           \n"); break;
			case  42: printf("Artillery            \n"); break;
			case  43: printf("Wagon Train          \n"); break;

			default:  printf("(%3d) unknown\n", colony[i].building_in_production); break;
		}

		for (int j = 0; j < sizeof (colony[i].unkb); ++j)
			printf("%02x ", colony[i].unkb[j]);
		printf("\n");

		printf("Stock;\n");
		for (int j = 0; j < 16; ++j)
			printf("  %11s: %3d\n", cargo_list[j], colony[i].stock[j]);
		printf("\n");

		for (int j = 0; j < sizeof (colony[i].unkd); ++j)
			printf("%02x ", colony[i].unkd[j]);
		printf("\n");

		printf("rebel ratio: %d/%d = %d\n",
			colony[i].rebel_dividend, colony[i].rebel_divisor,
			(colony[i].rebel_dividend * 100) / colony[i].rebel_divisor);
		printf("\n");

		if (just_this_one != -1)
			break;
	}

	printf("\n");
}

void print_unit(  const struct savegame::unit   *unit,   uint16_t unit_count, int just_this_one)
{
	printf("-- units --\n");

	int start = (just_this_one == -1) ? 0 : just_this_one;

	for (int i = start; i < unit_count; ++i) {
		printf("[%3d] (%3d, %3d): %-19s ", i, unit[i].x, unit[i].y, unit_type_list[unit[i].type]);

		printf("%-11s ", nation_list[unit[i].owner] );
		printf("m:%02x ", unit[i].moves);

		printf("tw:%d ", unit[i].turns_worked);

		switch (unit[i].type) {
			case  0: //savegame::unit::COLONIST:
			case  1: //savegame::unit::SOLDIER:
			case  2: //savegame::unit::PIONEER:
			case  3: //savegame::unit::MISSIONARY:
			case  4: //savegame::unit::DRAGOON:
			case  5: //savegame::unit::SCOUT:
			case  7: //savegame::unit::CONTINENTAL_CAVALRY:
			case  9: //savegame::unit::CONTINENTAL_ARMY:
			case 19: //savegame::unit::BRAVE:
				printf("%-22s", profession_list[unit[i].profession]);
				break;
			case 10: //savegame::unit::TREASURE:
				printf("%3d00 gold            ", unit[i].profession);
				break;
			case 13: //savegame::unit::CARAVEL:
			case 14: //savegame::uniT::MERCHANTMAN:
			case 15: //savegame::unit::GALEON:
				printf("%-22s", unit_type_list[unit[i].type]);
				assert(0 == unit[i].profession);
				break;
			default:
				printf("TYPE: %2d PROF: %2d     ", unit[i].type, unit[i].profession);
		}

		assert(unit[i].holds_occupied >= 0 &&
			unit[i].holds_occupied < 7 );

		printf("cargo_holds (%d) : [ %s:%3d, %s:%3d, %s:%3d, %s:%3d, %s:%3d, %s:%3d ]",
			unit[i].holds_occupied,
			(unit[i].holds_occupied > 0) ? cargo_list[unit[i].cargo_item_0] : "", (unit[i].holds_occupied > 0) ? unit[i].cargo_hold[0] : -1,
			(unit[i].holds_occupied > 1) ? cargo_list[unit[i].cargo_item_1] : "", (unit[i].holds_occupied > 1) ? unit[i].cargo_hold[1] : -1,
			(unit[i].holds_occupied > 2) ? cargo_list[unit[i].cargo_item_2] : "", (unit[i].holds_occupied > 2) ? unit[i].cargo_hold[2] : -1,
			(unit[i].holds_occupied > 3) ? cargo_list[unit[i].cargo_item_3] : "", (unit[i].holds_occupied > 3) ? unit[i].cargo_hold[3] : -1,
			(unit[i].holds_occupied > 4) ? cargo_list[unit[i].cargo_item_4] : "", (unit[i].holds_occupied > 4) ? unit[i].cargo_hold[4] : -1,
			(unit[i].holds_occupied > 5) ? cargo_list[unit[i].cargo_item_5] : "", (unit[i].holds_occupied > 5) ? unit[i].cargo_hold[5] : -1);

		printf("(%x) %02x %02x %02x ",
			unit[i].unk04,
			unit[i].unk05,
			unit[i].unk06,
			unit[i].unk07);

		for (int j = 0; j < sizeof (unit[i].unk08); ++j)
			printf("%02x ", unit[i].unk08[j]);

		printf("%3d %3d\n",
			unit[i].transport_chain.next_unit_idx,
			unit[i].transport_chain.prev_unit_idx);

		if (just_this_one != -1)
			break;
	}

	printf("\n");
}

void print_nation(const struct savegame::nation *nation, int just_this_one)
{
	printf("-- nations --\n");

	int start = (just_this_one == -1) ? 0 : just_this_one;

	for (int i = start; i < 4; ++i) {
		printf("%-11s, tax_rate: %2d\n", nation_list[i], nation[i].tax_rate);

		assert(nation[i].recruit_count <= 180); //does not go above 180

		printf("Recruit: (%3d)\n", nation[i].recruit_count);
		for (int j = 0; j < 3; ++j)
			printf("  %s\n", profession_list[ nation[i].recruit[j] ]);

		printf("%02x / %02x\n", nation[i].unk0, nation[i].unk1);
		assert(nation[i].unk1 == 0);

		for (int j = 0; j < sizeof (nation[i].unk2); ++j)
			printf("%02x ", nation[i].unk2[j]);
		printf("\n");

		printf("Liberty bell production: %3d (%4d)\n",
			nation[i].liberty_bells_last_turn,
			nation[i].liberty_bells_total);

		for (int j = 0; j < sizeof (nation[i].unk3); ++j)
			printf("%02x ", nation[i].unk3[j]);
		printf("\n");

		assert(nation[i].ffc_high == 0);
		printf("Founding fathers: %2d", nation[i].founding_father_count);
		if (nation[i].next_founding_father != -1)
			printf(", Next founding father: %s", founding_father_list[nation[i].next_founding_father] );
		printf("\n");


		printf("Villages burned: %d\n", nation[i].villages_burned);

		for (int j = 0; j < sizeof (nation[i].unk4); ++j)
			printf("%02x ", nation[i].unk4[j]);
		printf("\n");

		printf("Artillery count: %d\n", nation[i].artillery_count);
		
		
		for (int j = 0; j < sizeof (nation[i].unk5); ++j)
			printf("%02x ", nation[i].unk5[j]);
		printf("\n");

		printf("Gold: %5d, Crosses: %4d\n",
			nation[i].gold, nation[i].crosses);

		for (int j = 0; j < 4; ++j)
			printf("%d ", nation[i].unk6[j] );
		printf("\n");

		for (int j = 0; j < 8; ++j) {
			printf("Indian status - %-8s:", nation_list[INDIAN_OFFSET + j]);
			switch (nation[i].indian_relation[j]) {
				case savegame::nation::WAR:     printf("war\n");     break;
				case savegame::nation::PEACE:   printf("peace\n");   break;
				case savegame::nation::NOT_MET: printf("not met\n"); break;
				default: printf("unknown (%02x)\n", nation[i].indian_relation[j]);
			}
		}

		for (int j = 0; j < sizeof (nation[i].unk7); ++j)
			printf("%02x ", nation[i].unk7[j]);
		printf("\n");


		for (int j = 0; j < 16; ++j) {
			printf("%11s: %7s, ", cargo_list[j], nation[i].boycott_bitmap & (1 << j) ? "boycott" : "");
			printf("euro: %2d, %4d(%04x) nr, %5d gold, %4d tons, %4d tons2\n",
				nation[i].trade.euro_price[j],
				nation[i].trade.nr[j], nation[i].trade.nr[j],
				nation[i].trade.gold[j],
				nation[i].trade.tons[j],
				nation[i].trade.tons2[j]);
			//assert(nation[i].trade.tons[j] == nation[i].trade.tons2[j]); //under which circumstances are these two not equal?
		}

		printf("\n");

		if (just_this_one != -1)
			break;
	}
	printf("\n");
}

void print_tribe(const struct savegame::tribe  *tribe,  uint16_t tribe_count, int just_this_one)
{
	printf("-- tribes --\n");

	int start = (just_this_one == -1) ? 0 : just_this_one;

	for (int i = start; i < tribe_count; ++i) {
		printf("[%3d] (%3d, %3d): %2d %-11s :", i, tribe[i].x, tribe[i].y, tribe[i].population, nation_list[tribe[i].nation]);
		printf(" state: artillery(%d) learned(%d) capital(%d) scouted(%d) %d %d %d %d,",
			tribe[i].state.artillery, tribe[i].state.learned, tribe[i].state.capital, tribe[i].state.scouted,
			tribe[i].state.unk5, tribe[i].state.unk6, tribe[i].state.unk7, tribe[i].state.unk8);

		printf(" mission(%2d)", tribe[i].mission);
		printf(" unk1: %02x", tribe[i].unk1);
		printf(" f0: %d", tribe[i].flag_0);
		printf(" cargo_bought: %s", (tribe[i].last_cargo_bought != -1) ? cargo_list[ tribe[i].last_cargo_bought ] : "-1");
		printf(" cargo_sold: %s", (tribe[i].last_cargo_sold != -1) ? cargo_list[ tribe[i].last_cargo_sold ] : "-1");
		printf(" panic(%2d) ", tribe[i].panic);

		for (int j = 0; j < sizeof (tribe[i].unk2); ++j)
			printf("%02x ", tribe[i].unk2[j]);

		printf("%02x\n", tribe[i].population_loss_in_current_turn);

		if (just_this_one != -1)
			break;
	}
	printf("\n");
}

void print_indian(const struct savegame::indian_relations *ir, int just_this_one)
{
	printf("-- indian --\n");

	int start = (just_this_one == -1) ? 0 : just_this_one;

	for (int i = 0; i < 8; ++i) {
		printf("%-8s:", nation_list[INDIAN_OFFSET + i]);

		printf(" %02x %02x", ir[i].unk0, ir[i].unk1);
		printf(" %-12s", indian_level[ ir[i].level ]);

		for (int j = 0; j < sizeof (ir[i].unk2); ++j) {
			printf(" %02x", ir[i].unk2[j]);
		}

		printf(" %2d armed_braves?", ir[i].armed_braves);
		printf(" %2d horse_herds", ir[i].horse_herds);

		for (int j = 0; j < sizeof (ir[i].unk3); ++j) {
			printf(" %02x", ir[i].unk3[j]);
		}

		for (int j = 0; j < sizeof (ir[i].unk4); ++j) {
			printf(" %02x", ir[i].unk4[j]);
		}

		for (int j = 0; j < 4; ++j) {
			switch (j) {
				case 0: printf(" eng_met(%02x)", ir[i].meeting[j].met); break;
				case 1: printf(" fra_met(%02x)", ir[i].meeting[j].met); break;
				case 2: printf(" spa_met(%02x)", ir[i].meeting[j].met); break;
				case 3: printf(" dut_met(%02x)", ir[i].meeting[j].met); break;
				default: printf("ERROR"); break;
			}
		}

		for (int j = 0; j < sizeof (ir[i].unk5); ++j) {
			printf(" %02x", ir[i].unk5[j]);
		}

		for (int j = 0; j < 4; ++j) {
			switch (j) {
				case 0: printf(" eng_aggr(%3d)", ir[i].aggr[j].aggr); break;
				case 1: printf(" fra_aggr(%3d)", ir[i].aggr[j].aggr); break;
				case 2: printf(" spa_aggr(%3d)", ir[i].aggr[j].aggr); break;
				case 3: printf(" dut_aggr(%3d)", ir[i].aggr[j].aggr); break;
				default: printf("ERROR"); break;
			}
			assert(ir[i].aggr[j].aggr_high == 0);
		}
		printf("\n");

		printf("Stock;\n");
		for (int j = 0; j < 16; ++j)
			printf("  %11s: %3d\n", cargo_list[j], ir[i].stock[j]);
		printf("\n");

		if (just_this_one != -1)
			break;
	}
}

void print_stuff(const struct savegame::stuff *stuff)
{
	printf("-- stuff --\n");

	for (int i = 0; i < sizeof (stuff->unk15); ++i) {
		printf("%02x ", stuff->unk15[i]);
	}
	printf("\n");

	printf("decreasing_counter: %d\n", stuff->counter_decreasing_on_new_colony);
	printf("unk_short: %d\n", stuff->unk_short);
	printf("increasing_counter: %d\n", stuff->counter_increasing_on_new_colony);

	for (int i = 0; i < sizeof (stuff->unk_big); ++i) {
		if (i % 16 == 0)
			printf("\n[0x%03x]", i);
		printf(" %02x", stuff->unk_big[i]);
	}
	printf("\n");

	printf("Active unit: (%3d, %3d)\n", stuff->x, stuff->y);

	printf("Zoom level: ");
	switch (stuff->zoom_level) {
		case 0: printf(" 15 x 12"); break;
		case 1: printf(" 30 x 24"); break;
		case 2: printf(" 60 x 48"); break;
		case 3: printf("120 x 96"); break;
		default:
			printf("UNKNOWN: (%02x)", stuff->zoom_level);
			break;
	}
	printf("\n");

	printf("%02x\n", stuff->unk7);

	printf("Viewport: (%3d, %3d)\n", stuff->viewport_x, stuff->viewport_y);
}

void print_map(const struct savegame::map *map)
{
	printf("-- map --\n");

	for (int i = 0; i < 4; ++i) {
		for (int y = 0; y < 72; ++y) {
			for (int x = 0; x < 58; ++x)
				printf("%x", map->layer[i][x + (y * 58)].water ? map->layer[i][x + (y * 58)].tile + 9 : map->layer[i][x + (y * 58)].tile);
//				printf("%02x(%d)", map->layer[i][x + (y * 58)].full, map->layer[i][x + (y * 58)].tile);
			printf("\n");
		}
		printf("\n");
	}
}

void print_tail(const struct savegame::tail *tail)
{
	printf("-- tail --\n");

	for (int i = 0; i < sizeof (tail->unk); ++i) {
		if (i % 20 == 0)
			printf("\n");
		printf("%02x ", tail->unk[i]);
	}
	printf("\n");
}

void print_route(const struct savegame *sg, const struct savegame::trade_route *route, int just_this_one)
{
	printf("-- trade routes --\n");

	int start = (just_this_one == -1) ? 0 : just_this_one;

	for (int i = start; i < sg->head.trade_route_count; ++i) {
		printf("%-31s, type: %4s, entries: %d\n",
			route[i].name, route[i].type ? "sea" : "land",
			route[i].entries);

		for (int j = 0; j < route[i].entries; ++j) {
			printf("%d. %-24s",
				j, sg->colony[ route[i].entry[j].destination ].name);

			/* stupid string concatenation "trick" */
			printf(" | unloading: %d, [%s%s%s%s%s%s%s%s%s%s%s]",
				route[i].entry[j].unloading_size,
				(route[i].entry[j].unloading_size > 0) ? cargo_list[ route[i].entry[j].cargo[1].item_0 ] : "",
				(route[i].entry[j].unloading_size > 1) ? ", " : "",
				(route[i].entry[j].unloading_size > 1) ? cargo_list[ route[i].entry[j].cargo[1].item_1 ] : "",
				(route[i].entry[j].unloading_size > 2) ? ", " : "",
				(route[i].entry[j].unloading_size > 2) ? cargo_list[ route[i].entry[j].cargo[1].item_2 ] : "",
				(route[i].entry[j].unloading_size > 3) ? ", " : "",
				(route[i].entry[j].unloading_size > 3) ? cargo_list[ route[i].entry[j].cargo[1].item_3 ] : "",
				(route[i].entry[j].unloading_size > 4) ? ", " : "",
				(route[i].entry[j].unloading_size > 4) ? cargo_list[ route[i].entry[j].cargo[1].item_4 ] : "",
				(route[i].entry[j].unloading_size > 5) ? ", " : "",
				(route[i].entry[j].unloading_size > 5) ? cargo_list[ route[i].entry[j].cargo[1].item_5 ] : "");

			printf(" | loading: %d, [%s%s%s%s%s%s%s%s%s%s%s]",
				route[i].entry[j].loading_size,
				(route[i].entry[j].loading_size > 0) ? cargo_list[ route[i].entry[j].cargo[0].item_0 ] : "",
				(route[i].entry[j].loading_size > 1) ? ", " : "",
				(route[i].entry[j].loading_size > 1) ? cargo_list[ route[i].entry[j].cargo[0].item_1 ] : "",
				(route[i].entry[j].loading_size > 2) ? ", " : "",
				(route[i].entry[j].loading_size > 2) ? cargo_list[ route[i].entry[j].cargo[0].item_2 ] : "",
				(route[i].entry[j].loading_size > 3) ? ", " : "",
				(route[i].entry[j].loading_size > 3) ? cargo_list[ route[i].entry[j].cargo[0].item_3 ] : "",
				(route[i].entry[j].loading_size > 4) ? ", " : "",
				(route[i].entry[j].loading_size > 4) ? cargo_list[ route[i].entry[j].cargo[0].item_4 ] : "",
				(route[i].entry[j].loading_size > 5) ? ", " : "",
				(route[i].entry[j].loading_size > 5) ? cargo_list[ route[i].entry[j].cargo[0].item_5 ] : "");
			printf("\n");

			/* If this doesn't go unused, I'd like to know about it. */
			assert(route[i].entry[j].padding == 0);
		}
		printf("\n");

		if (just_this_one != -1)
			break;
	}
	printf("\n");
}

void dump(void *address, size_t bytes, const char *filename)
{
	FILE *fp = fopen(filename, "w");
	fwrite(address, bytes, 1, fp);
	fclose(fp);
}

// vim: ts=3
