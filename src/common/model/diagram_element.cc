/**
 * src/common/model/diagram_element.cc
 *
 * Copyright (c) 2021-2022 Bartek Kryza <bkryza@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "diagram_element.h"

#include "util/util.h"

#include <ostream>

namespace clanguml::common::model {

std::atomic_uint64_t diagram_element::m_nextId = 1;

diagram_element::diagram_element()
    : m_id{m_nextId++}
{
}

std::string diagram_element::alias() const
{
    return fmt::format("C_{:010}", m_id);
}

void diagram_element::add_relationship(relationship &&cr)
{
    if (cr.destination().empty()) {
        LOG_DBG("Skipping relationship '{}' - {} - '{}' due empty "
                "destination",
            cr.destination(), to_string(cr.type()), full_name(true));
        return;
    }

    if ((cr.type() == relationship_t::kInstantiation) &&
        (cr.destination() == full_name(true))) {
        LOG_DBG("Skipping self instantiation relationship for {}",
            cr.destination());
        return;
    }

    LOG_DBG("Adding relationship: '{}' - {} - '{}'", cr.destination(),
        to_string(cr.type()), full_name(true));

    if (!util::contains(relationships_, cr))
        relationships_.emplace_back(std::move(cr));
}

std::vector<relationship> &diagram_element::relationships()
{
    return relationships_;
}

const std::vector<relationship> &diagram_element::relationships() const
{
    return relationships_;
}

void diagram_element::append(const decorated_element &e)
{
    decorated_element::append(e);
}

inja::json diagram_element::context() const
{
    inja::json ctx;
    ctx["name"] = name();
    ctx["alias"] = alias();
    ctx["full_name"] = full_name(false);

    return ctx;
}

bool operator==(const diagram_element &l, const diagram_element &r)
{
    return l.full_name(false) == r.full_name(false);
}

std::ostream &operator<<(std::ostream &out, const diagram_element &rhs)
{
    out << "(" << rhs.name() << ", full_name=[" << rhs.full_name(false) << "])";

    return out;
}

}