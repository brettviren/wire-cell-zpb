#include "WireCellZpb/Converters.h"

#include "WireCellZpb/DataImps.h"

using namespace WireCell;

IDepo::pointer Zpb::convert(const wctzpb::Depo& depo)
{
    return std::make_shared<Zpb::Depo>(depo);
}
                     
wctzpb::Depo Zpb::convert(const IDepo::pointer& depo)
{
    wctzpb::Depo out;
    out.set_ident((uint64_t)depo.get()); // not guaranteed unique
    auto* pos = out.mutable_pos();
    pos->set_x(depo->pos().x());
    pos->set_y(depo->pos().y());
    pos->set_z(depo->pos().z());
    out.set_time(depo->time());
    out.set_charge(depo->charge());
    out.set_energy(depo->energy());
    out.set_trackid(depo->id());
    out.set_pdg(depo->pdg());
    out.set_prior((uint64_t)depo->prior().get());
    out.set_extent_long(depo->extent_long());
    out.set_extent_tran(depo->extent_tran());    
    return out;
}


IFrame::pointer Zpb::convert(const wctzpb::Frame& frame)
{
    return std::make_shared<Zpb::Frame>(frame);
}
                     
wctzpb::Frame Zpb::convert(const IFrame::pointer& frame)
{
    wctzpb::Frame out;
    // Fixme: this is not guaranteed unique over a "run" as the same
    // memory address could eventually be reused.  Maybe replace it
    // with a local counter?
    out.set_ident((uint64_t)frame.get()); 
    out.set_time(frame->time());
    out.set_tick(frame->tick());

    // traces
    auto tracesv = frame->traces();
    const int ntraces = tracesv->size();
    auto* pbtraces = out.mutable_traces();
    pbtraces->Reserve(ntraces);
    for (int itrace=0; itrace<ntraces; ++itrace) {
        const auto& trace = tracesv->at(itrace);
        auto* pbtr = pbtraces->Mutable(itrace);
        pbtr->set_channel(trace->channel());
        pbtr->set_tbin(trace->tbin());
        const auto& q = trace->charge();
        const int nq = q.size();
        auto* pbq = pbtr->mutable_samples();
        auto* pbqe = pbq->mutable_elements();
        pbqe->Reserve(nq);
        for (int ind=0; ind<nq; ++ind) {
            pbqe->Set(ind, q[ind]);
        }

    }

    // frame_tags
    for (const auto& ft : frame->frame_tags()) {
        out.add_frame_tags(ft);
    }

    // tagged_traces string->IntArray
    // trace_summarizes string->FloatArray
    auto* outtt = out.mutable_tagged_traces();
    auto* outts = out.mutable_trace_summaries();
    for (const auto& tag : frame->trace_tags()) {
        const auto& indices = frame->tagged_traces(tag);
        auto& pbtt = outtt->at(tag);
        int nind = indices.size();
        auto* pbtte = pbtt.mutable_elements();
        pbtte->Reserve(nind);
        for (int ind=0; ind<nind; ++ind) {
            pbtte->Set(ind, indices[ind]);
        }

        const auto& summary = frame->trace_summary(tag);
        int nsum = summary.size();
        if (nsum == 0) {
            continue;
        }
        if (nsum != nind) {
            // error/warning the two should be equal
        }
        auto& pbts = outts->at(tag);
        auto* pbtse = pbts.mutable_elements();
        for (int ind=0; ind<nsum; ++ind) {
            pbtse->Set(ind, summary[ind]);
        }
    }

    // channel_masks string->ChannelMasks
    auto* outcms = out.mutable_channel_masks();
    for (const auto& cmmit : frame->masks()) {
        const auto& tag = cmmit.first;
        for (const auto& ch_brl : cmmit.second) {
            int ch = ch_brl.first;
            auto& pbcms = outcms->at(tag);
            auto* pbbrls = pbcms.mutable_bin_range_lists();
            auto& pbbrl = pbbrls->at(ch);
            auto* beg = pbbrl.mutable_beg();
            auto* end = pbbrl.mutable_end();
            for (const auto& br : ch_brl.second) {
                beg->Add(br.first);
                end->Add(br.second);
            }
        }
    }

    return out;
}


