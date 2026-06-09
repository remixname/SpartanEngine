/*
Copyright(c) 2015-2026 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ======================
#include "pch.h"
#include "ResourceViewer.h"
#include "Resource/ResourceCache.h"
#include <variant>
//=================================

//= NAMESPACES ===============
using namespace std;
using namespace spartan;
using namespace spartan::math;
//============================

// Should eventually be moved to core, but kept here for now as it has no other dependencies.
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

namespace
{
    int resource_search_count = 0;

    void print_memory(uint64_t memory)
    {
        if (memory == 0)
        {
            ImGui::Text("0 Mb");
        }
        else if (memory < 1024)
        {
            ImGui::Text("%.4f Mb", static_cast<float>(memory) / 1000.0f / 1000.0f);
        }
        else
        {
            ImGui::Text("%.1f Mb", static_cast<float>(memory) / 1000.0f / 1000.0f);
        }
    }

    bool contains_search_ignore_case(const char* cstr_haystack, const char* cstr_needle)
    {
        string_view str_h = cstr_haystack;
        string_view str_n = cstr_needle;

        const auto it = ranges::search(str_h, str_n,
                                 [](unsigned char a, unsigned char b)
                                 {
                                     return std::tolower(a) == std::tolower(b);
                                 }).begin();

        return it != str_h.end();
    }

    bool is_resource_searched(const ResourceCache::ResourceInfo& resource_info, const char* cstr_needle)
    {
        for (size_t i = 0; i < ResourceCache::props_count; ++i)
        {
            std::string record_str;
            std::visit(overloaded
            {
                [&record_str](const std::string& prop) { record_str = prop; },
                [&record_str](uint64_t prop) { record_str = to_string(prop); }
            }, resource_info[i]);

            if (contains_search_ignore_case(record_str.c_str(), cstr_needle))
            {
                return true;
            }
        }
        return false;
    }
}

ResourceViewer::ResourceViewer(Editor* editor) : Widget(editor)
{
    m_title   = "Resource Viewer";
    m_visible = false;
}

void ResourceViewer::OnTickVisible()
{
    auto resources_info_list = ResourceCache::GetResourceInfoList();
    const float memory_usage = ResourceCache::GetMemoryUsage() / 1000.0f / 1000.0f;

    ImGui::Text("Resource count in scene: %zu, Memory usage: %d Mb", ResourceCache::GetResourceCount(), static_cast<uint32_t>(memory_usage));
    ImGui::Separator();
    static char search_buffer[128] = "";
    ImGui::InputTextWithHint("##resource_viewer_search", "Search by type, ID, name or path in case insensitive format", search_buffer, IM_ARRAYSIZE(search_buffer));
    if (search_buffer[0] != '\0')
    {
        ImGui::SameLine();
        ImGui::Text("%d result%s", resource_search_count, resource_search_count > 1 ? "s" : "");
        resource_search_count = 0;
    }
    ImGui::Separator();

    static ImGuiTableFlags flags =
        ImGuiTableFlags_Borders           | // Draw all borders.
        ImGuiTableFlags_RowBg             | // Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt (equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on each row manually)
        ImGuiTableFlags_Resizable         | // Allow resizing columns.
        ImGuiTableFlags_Reorderable       | // Allow reordering columns.
        ImGuiTableFlags_Sortable          | // Allow sorting rows.
        ImGuiTableFlags_ContextMenuInBody | // Right-click on columns body/contents will display table context menu. By default it is available in TableHeadersRow().
        ImGuiTableFlags_ScrollX           | // Enable horizontal scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size. Changes default sizing policy. Because this create a child window, ScrollY is currently generally recommended when using ScrollX.
        ImGuiTableFlags_ScrollY;            // Enable vertical scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size.

    static ImVec2 size = ImVec2(-1.0f);
    if (ImGui::BeginTable("##Widget_ResourceCache", 5, flags, size))
    {
        // Headers
        ImGui::TableSetupColumn("Type");
        ImGui::TableSetupColumn("ID");
        ImGui::TableSetupColumn("Name");
        ImGui::TableSetupColumn("Path");
        ImGui::TableSetupColumn("Size");
        ImGui::TableHeadersRow();

        // --- Sorting logic on column header click ---
        static int sorted_column                 = 1; // default sorting method by ID
        static ImGuiSortDirection sort_direction = ImGuiSortDirection_Ascending;

        if (ImGuiTableSortSpecs* table_sort_specs = ImGui::TableGetSortSpecs())
        {
            if (table_sort_specs->SpecsDirty)
            {
                const ImGuiTableColumnSortSpecs* spec = &table_sort_specs->Specs[0];
                sorted_column = spec->ColumnIndex;
                sort_direction = spec->SortDirection;
                table_sort_specs->SpecsDirty = false;
            }
        }
        SP_ASSERT(sorted_column >= 0 && sorted_column < ResourceCache::props_count);
        ranges::sort(resources_info_list, [](const ResourceCache::ResourceInfo& a, const ResourceCache::ResourceInfo& b)
        {
            return sort_direction == ImGuiSortDirection_Ascending
                                ? a[sorted_column] < b[sorted_column]
                                : a[sorted_column] > b[sorted_column];

            }
        );

        // --- Draw Row Data ---
        for (const ResourceCache::ResourceInfo& resource_info : resources_info_list)
        {
                if (search_buffer[0] != '\0')
                {
                if (!is_resource_searched(resource_info, search_buffer))
                    {
                        continue;
                    }

                    resource_search_count++;
                }

                // Switch row
                ImGui::TableNextRow();

            for (int i = 0; i < ResourceCache::props_count; ++i)
            {
                ImGui::TableSetColumnIndex(i);
                std::visit(overloaded
                    {
                        [](const std::string& prop) { ImGui::Text(prop.c_str()); },
                        [](uint64_t prop) { ImGui::Text(to_string(prop).c_str()); }
                    }, resource_info[i]);
            }
        }

        ImGui::EndTable();
    }
}
