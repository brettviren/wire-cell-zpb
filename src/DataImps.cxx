#include "WireCellZpb/DataImps.h"
#include <google/protobuf/repeated_field.h>
using namespace WireCell;

Zpb::Depo::Depo(const wctzpb::Depo& depo, IDepo::pointer prior)
    : SimpleDepo(depo.time(),
                 Point(depo.pos().x(), depo.pos().y(), depo.pos().z()),
                 depo.charge(),
                 prior,
                 depo.extent_long(),
                 depo.extent_tran(),
                 depo.trackid(),
                 depo.pdg(),
                 depo.energy())
{
}



Zpb::Trace::Trace(const wctzpb::Trace& trace)
    : SimpleTrace(trace.channel(), 
                  trace.tbin(), 
                  trace.samples().elements_size())
{
    auto& q = charge();
    const int nsamples = q.size();
    const auto& samples = trace.samples();
    for (int ind=0; ind<nsamples; ++ind) {
        const float qq = samples.elements(ind);
        q[ind] = qq;
    }
}

static
ITrace::shared_vector make_traces(const wctzpb::Frame& frame)
{
    auto ret = new std::vector<ITrace::pointer>;
    for (auto& pbtrace : frame.traces()) {
        ret->push_back(std::make_shared<Zpb::Trace>(pbtrace));
    }
    return ITrace::shared_vector(ret);
}

static
Waveform::ChannelMaskMap make_cmm(const wctzpb::Frame& pbframe)
{
    Waveform::ChannelMaskMap ret;
    for (const auto& tag_cms : pbframe.channel_masks()) {
        Waveform::ChannelMasks cm;
        const auto& pbcms = tag_cms.second;
        for (const auto& ch_brl : pbcms.bin_range_lists()) {
            Waveform::BinRangeList brl;   // vector<pair<int,int>>
            const auto& pbbrl = ch_brl.second;
            int nbr = pbbrl.beg_size();
            for (int ind=0; ind<nbr; ++ind) {
                brl.push_back(Waveform::BinRange(pbbrl.beg(ind),
                                                 pbbrl.end(ind)));
            }
            cm[ch_brl.first] = brl;
        }
        ret[tag_cms.first] = cm;
    }
    return ret;                 // works as temporary?
}

Zpb::Frame::Frame(const wctzpb::Frame& pbframe)
    : SimpleFrame(pbframe.ident(),
                  pbframe.time(),
                  make_traces(pbframe),
                  pbframe.tick(),
                  make_cmm(pbframe))
{
    for (const auto& tag: pbframe.frame_tags()) {
        tag_frame(tag);
    }

    const auto& pbts = pbframe.trace_summaries();
    for (const auto& tag_inds: pbframe.tagged_traces()) {
        const auto& tag = tag_inds.first;
        const auto& pbind = tag_inds.second.elements();
        const auto& pbsum = pbts.at(tag).elements();

        IFrame::trace_list_t indices(pbind.begin(), pbind.end());
        IFrame::trace_summary_t summary(pbsum.begin(), pbsum.end());
        tag_traces(tag, indices, summary);
    }
    
}
