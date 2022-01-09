include "tribes/scripting/help/time_strings.lua"

push_textdomain("scenario_atl02.wmf")

local result = {
   atlanteans = {
      buildings = {
         {
            name = "atlanteans_trading_post",
            helptexts = {
               -- TRANSLATORS: Purpose helptext for an atlantean market site: Trading post
               purpose = pgettext("atlanteans_building", "Allows sharing goods with other players.")
            }
         },
         {
            name = "atlanteans_scouts_house1",
            helptexts = {
               no_scouting_building_connected = pgettext("atlanteans_building", "You need to connect this flag to a scout’s house before you can send a scout here."),
               -- TRANSLATORS: Purpose helptext for an atlantean production site: Scout's House
               purpose = pgettext("building", "Explores unknown territory.")
            }
         },
      },
      workers_order = {
         {
            { name = "atlanteans_trader" }
         }
      },
      wares_order = {
         {
            {
               name= "coin_wood",
               default_target_quantity = 5,
               preciousness = 1
            },
            {
               name= "coin_copper",
               default_target_quantity = 4,
               preciousness = 1
            },
            {
               name= "coin_silver",
               default_target_quantity = 3,
               preciousness = 1
            },
            {
               name= "coin_gold",
               default_target_quantity = 2,
               preciousness = 1
            }
         },
      }
   },
   barbarians = {
      immovables = {
         {
            name = "barbarians_trade_pole",
            helptexts = {}
         },
      }
   }
}

pop_textdomain()
return result
