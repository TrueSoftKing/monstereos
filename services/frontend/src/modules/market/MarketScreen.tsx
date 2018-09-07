import * as React from "react"
import { connect } from "react-redux"
import { State, doLoadOffers, doLoadMyMonsters } from "../../store"
import { getEosAccount } from "../../utils/scatter"
import { OfferProps } from "./market"

import PageContainer from "../shared/PageContainer"
import TitleBar from "../shared/TitleBar"
import OfferCard from "./OfferCard"
import NewOfferModal from "./NewOfferModal"

// Bid is Off for now due to monsters autocomplete.
// no way to select all monsters in memory? demux to the rescue
// import NewBidModal from "./NewBidModal"

interface Props {
  eosAccount: string,
  offers:OfferProps[],
  globalConfig: any,
  dispatchDoLoadOffers: any,
  dispatchDoLoadMyMonsters: any
}

interface ReactState {
  showNewOfferModal: boolean,
  showNewBidModal: boolean
}

class MarketScreen extends React.Component<Props, ReactState> {

  public state = {
    showNewOfferModal: false,
    showNewBidModal: false
  }

  public render() {

    const { eosAccount } = this.props

    if (eosAccount) {
      return this.renderMarket(eosAccount)
    } else {
      return <PageContainer>
          <div>Ooopss... looks like you are not identified</div>
        </PageContainer>
    }
  }

  private renderMarket(eosAccount: string) {

    const { offers, dispatchDoLoadOffers, dispatchDoLoadMyMonsters } = this.props
    // const { showNewOfferModal, showNewBidModal } = this.state
    const { showNewOfferModal } = this.state

    const subHeader = (<small className="is-hidden-mobile">
     {offers.length} offers
      </small>)

    const refetchOffers = () => {
      setTimeout(() => dispatchDoLoadOffers(), 500)
    }

    const refetchMonsters = () => {
      setTimeout(() => dispatchDoLoadMyMonsters(), 500)
    }

    const newOfferButton = (
      <a
        className="button is-success"
        onClick={() => this.setState({showNewOfferModal: true})}>
        New Offer
      </a>
    )

    const newOfferClosure = (doRefetch: boolean) => {
      this.setState({showNewOfferModal: false})
      if (doRefetch) {
        refetchOffers()
      }
    }

    // const newBidButton = (
    //   <a
    //     className="button is-success"
    //     onClick={() => this.setState({showNewBidModal: true})}>
    //     New Bid
    //   </a>
    // )

    // const newBidClosure = (doRefetch: boolean) => {
    //   this.setState({showNewBidModal: false})
    //   if (doRefetch) {
    //     refetchOffers()
    //   }
    // }

    return (
      <PageContainer>
        <TitleBar
          title="Market for Monsters"
          notMobile
          menu={[subHeader, newOfferButton]} />
          <OfferList
            offers={offers}
            update={refetchMonsters} />
          {showNewOfferModal &&
          <NewOfferModal
            closeModal={newOfferClosure}
          />}
          {/* {showNewBidModal &&
          <NewBidModal
            closeModal={newBidClosure}
          />} */}
      </PageContainer>
    )
  }
}

const OfferList = ({ offers, update }: any) => (
  <div className="columns is-multiline">
    {offers.map((offer: any) => (
      <OfferCard
        key={offer.id}
        offer={offer}
        requestUpdate={update}/>
    ))}
  </div>
)

const isValidForUser = (user:string) => (offer:OfferProps) => {
  return offer.user === user || (offer.monster.name.length > 0 &&
      (isValidOffer(offer) || isValidBid(offer)))
}

const isValidOffer = (offer:OfferProps) => {
  return offer.type in [1, 10, 11] && offer.monster.owner === offer.user
}

const isValidBid = (offer:OfferProps) => {
  return offer.type in [2, 12] && offer.monster.owner !== offer.user
}

const mapStateToProps = (state: State) => {
  const eosAccount = getEosAccount(state.identity)

  return {
    eosAccount,
    offers: state.offers.filter(isValidForUser(eosAccount.name)),
    globalConfig: state.globalConfig,
  }
}

const mapDispatchToProps = {
  dispatchDoLoadOffers: doLoadOffers,
  dispatchDoLoadMyMonsters: doLoadMyMonsters
}

export default connect(mapStateToProps, mapDispatchToProps)(MarketScreen)