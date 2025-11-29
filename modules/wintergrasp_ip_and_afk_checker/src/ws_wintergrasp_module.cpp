// and to kick AFK players periodically from the battle.

#include "ScriptMgr.h"
#include "Player.h"
#include "Chat.h"
#include "Battleground.h"
#include "BattlegroundMgr.h"
#include "WorldSession.h"
#include "Config.h"
#include "BattlefieldMgr.h"
#include "BattlefieldWG.h"
#include "Battlefield.h"

std::unordered_set<std::string> whitelistedIps;

class WintergraspIPEnforcerGroup : public GroupScript
{
public:
    WintergraspIPEnforcerGroup() : GroupScript("WintergraspIPEnforcerGroup") {}

    void OnAddMember(Group* group, ObjectGuid guid)
    {
        //Player* player = ObjectAccessor::FindPlayer(guid);
        //if (!player) return;
        //ChatHandler(player->GetSession()).PSendSysMessage("[DEBUG] Entró a OnAddMember");
        //if (!player->GetSession()) {
        //    ChatHandler(player->GetSession()).PSendSysMessage("[DEBUG] No tiene sesión");
        //    return;
        //}
        //if (player->GetZoneId() != 4197) {
        //    ChatHandler(player->GetSession()).PSendSysMessage("[DEBUG] No está en Wintergrasp");
        //    return;
        //}

        //// Solo aplicar la lógica si el grupo es de banda (raid)
        //if (!group || !group->isRaidGroup()) {
        //    ChatHandler(player->GetSession()).PSendSysMessage("[DEBUG] No es grupo de banda");
        //    return;
        //}

        //ChatHandler(player->GetSession()).PSendSysMessage("[DEBUG] Validando IP duplicada en banda");
        //// Obtener la IP del jugador
        //const std::string& newIP = player->GetSession()->GetRemoteAddress();
        //uint32 sameIpCount = 0;

        //// Recorrer todos los jugadores en el mapa
        //for (const auto& ref : player->GetMap()->GetPlayers())
        //{
        //    auto otherGuid = ref.GetSource()->GetGUID();
        //    if (Player* other = ObjectAccessor::FindPlayer(otherGuid))
        //    {
        //        if (other->GetSession() && other->GetSession()->GetRemoteAddress() == newIP)
        //        {
        //            sameIpCount++;
        //        }
        //    }
        //}

        //ChatHandler(player->GetSession()).PSendSysMessage("[DEBUG] Jugadores con la misma IP: %u", sameIpCount);
        //// Si hay más de un jugador con la misma IP en el mapa y en raid, expulsar
        //if (sameIpCount > 1)
        //{
        //    ChatHandler(player->GetSession()).PSendSysMessage("No puedes unirte a un grupo de banda en Conquista del Invierno si hay más de un jugador bajo tu misma red. Si esta acción se repite, serás penalizado por multicuenta.");
        //    // Sacar al jugador del grupo antes de teletransportar
        //    if (player->GetGroup())
        //    {
        //        player->RemoveFromGroup();
        //    }
        //    player->TeleportTo(player->GetStartPosition());
        //}
    }
};


class WintergraspAFKKickerScript : public PlayerScript
{
public:
    WintergraspAFKKickerScript() : PlayerScript("WintergraspAFKKickerScript")
    {
    }

    void OnPlayerUpdate(Player* player, uint32 diff) override
    {
        if (player->GetZoneId() != 4197)
            return;

        Battlefield* Bf = sBattlefieldMgr->GetBattlefieldToZoneId(4197);

        if (!Bf->IsWarTime())
            return;

        static uint32 afkTimer = 0;
        afkTimer += diff;

        if (afkTimer < 1500)
            return;

        afkTimer = 0;

        // Leer el valor de honor a quitar desde la config
        int32 honorToRemove = uint32(sConfigMgr->GetOption<int32>("WGChecker.AFKHonorRemove", 10000));
        //std::cout << "Honor a quitar por AFK: " << honorToRemove << std::endl;

        // imprimir en consola el nombre del jugador y tambien el diff
        //std::cout << "Jugador: " << player->GetName() << ", Diff: " << diff << std::endl;

        if (player->HasPlayerFlag(PLAYER_FLAGS_AFK)) {
            ChatHandler(player->GetSession()).PSendSysMessage("Has sido expulsado de Conquista del Invierno por estar AFK. Pierdes puntos de honor.", honorToRemove);
            Bf->HandlePlayerLeaveZone(player, 4197);
            SacarPlayerDeMapaWG(player);
            player->ModifyHonorPoints(-honorToRemove);
        }

    }

private:
    void SacarPlayerDeMapaWG(Player* player) {
        if (!player)
            return;

        const uint8 teleportMode = sConfigMgr->GetOption<uint8>("WGChecker.TeleportMode", 0);

        switch (teleportMode) {
        case 0: // Default (Wintergrasp)
            player->TeleportTo(571, 5728.117f, 2714.346f, 697.733f, 0.0f);
            break;
        case 1: // Dalaran
            player->TeleportTo(571, 5807.98f, 588.487f, 660.94f, 1.66594f);
            break;
        case 2: // Lugar de inicio (Homebind)
            player->TeleportTo(player->GetStartPosition());
            break;
        case 3: // Posición personalizada
        {
            const float posX = sConfigMgr->GetOption<float>("WGChecker.TeleportModeX", 0.0f);
            const float posY = sConfigMgr->GetOption<float>("WGChecker.TeleportModeY", 0.0f);
            const float posZ = sConfigMgr->GetOption<float>("WGChecker.TeleportModeZ", 0.0f);
            const float posO = sConfigMgr->GetOption<float>("WGChecker.TeleportModeO", 0.0f);
            const uint32 mapId = sConfigMgr->GetOption<uint32>("WGChecker.TeleportModeMap", 0);

            if (mapId != 0) { // Validación básica del MapID
                player->TeleportTo(mapId, posX, posY, posZ, posO);
            }
            else {
                // Fallback seguro si el MapID no está configurado
                player->TeleportTo(player->GetStartPosition());
            }
            break;
        }
        default: // Cualquier otro valor → Homebind
            player->TeleportTo(player->GetStartPosition());
            break;
        }
    }
};

class BattlefieldAccessor : public Battlefield {
public:
    const GuidUnorderedSet(&GetPlayersInWar() const)[PVP_TEAMS_COUNT] {
        return m_PlayersInWar; // Accede al miembro protegido
        }
};

class WGPacketScripts : public ServerScript {
public:
    WGPacketScripts() : ServerScript("WGPacketScripts") {}


    // Recibe paquete el servidor
    bool CanPacketReceive(WorldSession* session, WorldPacket& packet) {
        if (packet.GetOpcode() == CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE) {
            uint32 BattleId;
            uint8 Accepted;

            //std::cout << "Recibiendo paquete CMSG_BATTLEFIELD_MGR_ENTRY_INVITE_RESPONSE" << std::endl;

            packet >> BattleId >> Accepted;

            Battlefield* Bf = sBattlefieldMgr->GetBattlefieldByBattleId(BattleId);
            if (!Bf)
                return false;

            //std::cout << "BattleId: " << BattleId << ", Accepted: " << (int)Accepted << std::endl;

            // Si el jugador acepta la invitación hacer las acciones necesarias
            if (Accepted) {
                BattlefieldAccessor* BfAccessor = reinterpret_cast<BattlefieldAccessor*>(Bf);

                const auto& playersInWar = BfAccessor->GetPlayersInWar();

                if (HasSameIpInMap(session, playersInWar)) {
                    ChatHandler(session).PSendSysMessage("No Puedes Acceder a Conquista del Invierno si hay más de un jugador bajo tu misma red. Si esta acción se repite, serás penalizado por multicuenta.");
                    // Aqui vamos a teleportear segun las coordenadas dadas.
                    SacarPlayerDeMapaWG(session->GetPlayer());

                    return false;
                }
            }

        }

        return true;
    }
private:
    bool HasSameIpInMap(WorldSession* session, const GuidUnorderedSet(&playersInWar)[PVP_TEAMS_COUNT]) {
        if (!session || !session->GetPlayer())
            return false;

        const std::string& currentIp = session->GetRemoteAddress();
        Player* player = session->GetPlayer();
        ObjectGuid playerGuid = player->GetGUID();

        if (whitelistedIps.find(currentIp) != whitelistedIps.end()) {
            return false;
        }

        for (const auto& team : playersInWar) {
            for (const ObjectGuid& otherGuid : team) {
                if (otherGuid == playerGuid)
                    continue;

                Player* otherPlayer = ObjectAccessor::FindPlayer(otherGuid);
                if (!otherPlayer)
                    continue;

                if (otherPlayer->GetSession()->GetRemoteAddress() == currentIp) {
                    return true;
                }
            }
        }
        return false;
    }

    void SacarPlayerDeMapaWG(Player* player) {
        if (!player)
            return;

        const uint8 teleportMode = sConfigMgr->GetOption<uint8>("WGChecker.TeleportMode", 0);

        switch (teleportMode) {
        case 0: // Default (Wintergrasp)
            player->TeleportTo(571, 5728.117f, 2714.346f, 697.733f, 0.0f);
            break;
        case 1: // Dalaran
            player->TeleportTo(571, 5807.98f, 588.487f, 660.94f, 1.66594f);
            break;
        case 2: // Lugar de inicio (Homebind)
            player->TeleportTo(player->GetStartPosition());
            break;
        case 3: // Posición personalizada
        {
            const float posX = sConfigMgr->GetOption<float>("WGChecker.TeleportModeX", 0.0f);
            const float posY = sConfigMgr->GetOption<float>("WGChecker.TeleportModeY", 0.0f);
            const float posZ = sConfigMgr->GetOption<float>("WGChecker.TeleportModeZ", 0.0f);
            const float posO = sConfigMgr->GetOption<float>("WGChecker.TeleportModeO", 0.0f);
            const uint32 mapId = sConfigMgr->GetOption<uint32>("WGChecker.TeleportModeMap", 0);

            if (mapId != 0) { // Validación básica del MapID
                player->TeleportTo(mapId, posX, posY, posZ, posO);
            }
            else {
                // Fallback seguro si el MapID no está configurado
                player->TeleportTo(player->GetStartPosition());
            }
            break;
        }
        default: // Cualquier otro valor → Homebind
            player->TeleportTo(player->GetStartPosition());
            break;
        }
    }
};


class WGWorld : public WorldScript {
public:
    WGWorld() : WorldScript("WGWorld") { }

    void OnAfterConfigLoad(bool /*reload*/) override {
        // Cargar las IPs permitidas desde el archivo de configuración
        std::string ipList = sConfigMgr->GetOption<std::string>("WGChecker.WhiteList", "");

        whitelistedIps.clear(); // Limpiar el conjunto antes de cargar nuevas IPs
        if (!ipList.empty()) {
            std::istringstream iss(ipList);
            std::string ip;
            while (std::getline(iss, ip, ';')) {
                whitelistedIps.insert(ip);
            }
        }

    }
};


void Addwintergrasp_ip_and_afk_checkerScripts()
{
    new WintergraspIPEnforcerGroup();
    new WintergraspAFKKickerScript();
    new WGPacketScripts();
    new WGWorld();
}
