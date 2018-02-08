// Copyright (c) 2009-2012 The Bitcoin developers
// Copyright (c) 2017 The Signatum Project www.signatum.io
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/assign/list_of.hpp> // for 'map_list_of()'
#include <boost/foreach.hpp>

#include "checkpoints.h"

#include "txdb.h"
#include "main.h"
#include "uint256.h"


static const int nCheckpointSpan = 500;

namespace Checkpoints
{
    typedef std::map<int, uint256> MapCheckpoints;

    static MapCheckpoints mapCheckpoints =
        boost::assign::map_list_of
        (      0,     uint256("0x000002436db576ce4490d4ca7200eb65831c6d307a6dbfce866a9b791ff9f124") )
        (  10500,     uint256("0xaa04c3a5ffc9f33db2f9a08aeab561e3ffd86138528de5a3760a865b1004e2d5") )		
		(  25000,     uint256("0x4f6bf8b66093d45bc8980682863ef8df9b7013ac4353e8ec08e5fe196150c977") )
		(  50000,     uint256("0xddc172184933b1f1485526da9bac3992ec08e185f457dab69dd7042fb662e218") )
		(  55750,     uint256("0xa3f0275620d65ef646565328223e74d74ad7b3a6870a52e719aff368492d2cd7") )
		(  55751,     uint256("0xaa04ae63f7c8fb725a566f4c0adc5b5e13d695d733dd024278dc4733233f7ce9") )
		(  55752,     uint256("0x0754e54f6479af2a876f596de12c30212f60609ef3e2bf912a7e71cc651c7c8c") )
		(  60000,     uint256("0xd4d19c36c9c343b6dbe4ac782cdb8ec9dfe86827fb3274ff1b3e8de380f26228") )
		(  65000,     uint256("0x3d3bde294fc1d412bcb26fad3268401c780a62fe1366e7fe0c69d60c13421f46") )
		(  74500,     uint256("0x9f361050cf9d7ad4e7fe6e92a69c46c43d10dc4ed5e85af1ea7add7e58a2c758") )
		(  75000,     uint256("0x63f5714886f200b0a0f5c4bccaaaf47fa89066d81b1ceaf24ae3de9bdf52d6ef") )
		(  110308,    uint256("0x0bbad9e5a01821f59f1839ef3f387de087ca89947b234de8366e685ed94728f7") )
		(  110309,    uint256("0x09001a81b1c7e0d084fc66e43f8a56292536a697df0e2af06d498b1d131ece59") )
        ;

    // TestNet has no checkpoints
    static MapCheckpoints mapCheckpointsTestnet;

    bool CheckHardened(int nHeight, const uint256& hash)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        MapCheckpoints::const_iterator i = checkpoints.find(nHeight);
        if (i == checkpoints.end()) return true;
        return hash == i->second;;
    }

    int GetTotalBlocksEstimate()
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        if (checkpoints.empty())
            return 0;
        return checkpoints.rbegin()->first;
    }

    CBlockIndex* GetLastCheckpoint(const std::map<uint256, CBlockIndex*>& mapBlockIndex)
    {
        MapCheckpoints& checkpoints = (TestNet() ? mapCheckpointsTestnet : mapCheckpoints);

        BOOST_REVERSE_FOREACH(const MapCheckpoints::value_type& i, checkpoints)
        {
            const uint256& hash = i.second;
            std::map<uint256, CBlockIndex*>::const_iterator t = mapBlockIndex.find(hash);
            if (t != mapBlockIndex.end())
                return t->second;		  
        }
        return NULL;
    }

    // Automatically select a suitable sync-checkpoint 
    const CBlockIndex* AutoSelectSyncCheckpoint()
    {
        const CBlockIndex *pindex = pindexBest;
        // Search backward for a block within max span and maturity window
        while (pindex->pprev && pindex->nHeight + nCheckpointSpan > pindexBest->nHeight)
            pindex = pindex->pprev;
        return pindex;
    }

    // Check against synchronized checkpoint
    bool CheckSync(int nHeight)
    {
        const CBlockIndex* pindexSync = AutoSelectSyncCheckpoint();

        if (nHeight <= pindexSync->nHeight)
            return false;
        return true;
    }
}