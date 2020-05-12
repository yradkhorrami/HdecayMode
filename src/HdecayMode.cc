#include "HdecayMode.h"
#include <iostream>
#include <vector>
#include <string>
#include <EVENT/LCCollection.h>
#include <EVENT/MCParticle.h>

using namespace lcio ;
using namespace marlin ;
using namespace std ;

HdecayMode aHdecayMode ;

HdecayMode::HdecayMode() :

	Processor("HdecayMode"),
	m_isDecayedTob(0),
	m_isDecayedToc(0),
	m_isDecayedToother(0),
	m_ISREnergy(0.f)
{

	_description = "HdecayMode identifies Higgs decay mode and finds ISR in generator level" ;

	registerInputCollection( 	LCIO::MCPARTICLE,
					"MCParticleCollection" ,
					"Name of the MCParticle collection"  ,
					m_mcParticleCollection,
					std::string("MCParticle")
					);

	registerOutputCollection( 	LCIO::MCPARTICLE,
					"HdecayModeCollection",
					"Collection of Higgs decay mode plus ISR",
					m_HdecayModeCollection,
					std::string("HdecayMode")
					);
}

void HdecayMode::init()
{

	streamlog_out(DEBUG) << "   init called  " << std::endl ;
	m_isDecayedTob = 0;
	m_isDecayedToc = 0;
	m_isDecayedToother = 0;
	m_ISREnergy = 0.;
	this->Clear();

}

void HdecayMode::Clear()
{
	m_isDecayedTob = 0;
	m_isDecayedToc = 0;
	m_isDecayedToother = 0;
	m_ISREnergy = 0.;
}

void HdecayMode::processRunHeader()
{

}

void HdecayMode::processEvent( LCEvent *pLCEvent )
{
	try
	{
		this->Clear();
		const EVENT::LCCollection *mcpCollection = pLCEvent->getCollection(m_mcParticleCollection);
		streamlog_out(DEBUG) << "Processing event " << pLCEvent->getEventNumber() << std::endl;
		m_col_HDecayMode = new LCCollectionVec(LCIO::MCPARTICLE);
		for (int i = 0; i < mcpCollection->getNumberOfElements(); ++i)
		{
			const EVENT::MCParticle *pMCParticle = dynamic_cast<EVENT::MCParticle*>(mcpCollection->getElementAt(i));
			if ( pMCParticle->getPDG() == 25 && (pMCParticle->getDaughters()).size() == 2 )
			{
				if ( abs((pMCParticle->getDaughters()[0])->getPDG()) == 5 && (pMCParticle->getDaughters()[0])->getPDG() / (pMCParticle->getDaughters()[1])->getPDG() == -1 ) 
				{
					m_isDecayedTob = 1;
					streamlog_out(DEBUG) << "Higgs decays to bbbar!" << std::endl;
				}
				else if ( abs((pMCParticle->getDaughters()[0])->getPDG()) == 4 && (pMCParticle->getDaughters()[0])->getPDG() / (pMCParticle->getDaughters()[1])->getPDG() == -1 )
				{
					m_isDecayedToc = 1;
					streamlog_out(DEBUG) << "Higgs decays to ccbar!" << std::endl;
				}
				else
				{
					m_isDecayedToother = 1;
					streamlog_out(DEBUG) << "Higgs decays to neither bbbar nor ccbar!: ( " << (pMCParticle->getDaughters()[0])->getPDG() << " , " << (pMCParticle->getDaughters()[1])->getPDG() << " )" << std::endl;
				}
			}
			if ( pMCParticle->getPDG() == 22 && (pMCParticle->getParents()).size() == 1 && abs((pMCParticle->getParents()[0])->getPDG()) == 11 )
			{
				if ( i == 6 || i == 7 )
				{
					m_ISREnergy += pMCParticle->getEnergy();
				}
			}
		}
		streamlog_out(DEBUG) << "ISR energy is " << m_ISREnergy << " GeV" << std::endl;
		m_col_HDecayMode->parameters().setValue("isDecayedTob", (int)m_isDecayedTob);		
		m_col_HDecayMode->parameters().setValue("isDecayedToc", (int)m_isDecayedToc);		
		m_col_HDecayMode->parameters().setValue("isDecayedToc", (int)m_isDecayedToother);		
		m_col_HDecayMode->parameters().setValue("ISREnergy", (float)m_ISREnergy);
		pLCEvent->addCollection(m_col_HDecayMode, m_HdecayModeCollection);		
	}
	catch(...)
	{
		streamlog_out(WARNING) << "Could not extract MCParticle collection: " << m_mcParticleCollection << std::endl;
	}
	
}

void HdecayMode::check()
{
    // nothing to check here - could be used to fill checkplots in reconstruction processor
}


void HdecayMode::end()
{

}



