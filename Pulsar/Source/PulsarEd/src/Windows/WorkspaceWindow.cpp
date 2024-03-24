﻿#include <PulsarEd/Windows/WorkspaceWindow.h>

#include "CoreLib.Platform/FolderWatch.h"
#include "CoreLib.Platform/System.h"
#include "CoreLib.Platform/Window.h"
#include "EditorAppInstance.h"
#include "Importers/AssetImporter.h"
#include "Windows/EditorWindowManager.h"

#include <Pulsar/IconsForkAwesome.h>
#include <PulsarEd/AssetDatabase.h>
#include <PulsarEd/AssetProviders/AssetProvider.h>
#include <PulsarEd/Menus/Types.h>
#include <PulsarEd/Workspace.h>

#include <PulsarEd/Utils/AssetUtil.h>

#include <PulsarEd/Menus/MenuRenderer.h>
#include <imgui/imgui_internal.h>

namespace pulsared
{
    namespace PImGui
    {
        struct FileButtonContext
        {
            const char* str;
            ImTextureID texture_id;
            ImVec2 size;
            float label_height;
            bool selected;
            bool is_dirty;
            ImTextureID dirty_texid;
            void* user_data;
            std::function<void(void*)> tootip;
            ImVec2 uv0 = ImVec2(0, 0);
            ImVec2 uv1 = ImVec2(1, 1);
            bool is_input_mode;
            bool is_folder;
        };

        static bool FileButton(FileButtonContext* ctx)
        {
            using namespace ImGui;

            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiID id = window->GetID(ctx->str);

            const ImVec2 padding = g.Style.FramePadding;

            ImVec2 offset = ctx->size;
            offset.y += ctx->label_height;

            ImRect pic_bb(window->DC.CursorPos, window->DC.CursorPos + ctx->size + padding * 2.0f);
            ImRect dirty_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2{16, 16});

            const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + offset + padding * 2.0f);
            ItemSize(bb);
            if (!ItemAdd(bb, id))
                return false;

            bool hovered{}, held{};
            bool pressed{};

            pressed = ButtonBehavior(bb, id, &hovered, &held, ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_PressedOnDoubleClick);

            ImVec4 bg_col = {0, 0, 0, 0};
            ImVec4 tint_col = {1, 1, 1, 1};

            // Render
            ImU32 col{};
            if (ctx->selected)
            {
                col = GetColorU32(ImGuiCol_ButtonHovered);
            }
            else
            {
                col = GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered
                                                                                      : ImGuiCol_Button);
            }
            RenderNavHighlight(bb, id);
            RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, g.Style.FrameRounding));

            if (bg_col.w > 0.0f)
                window->DrawList->AddRectFilled(bb.Min + padding, bb.Max - padding, GetColorU32(bg_col));
            window->DrawList->AddImage(ctx->texture_id, pic_bb.Min + padding, pic_bb.Max - padding, ctx->uv0, ctx->uv1, GetColorU32(tint_col));

            if (ctx->is_dirty)
            {
                window->DrawList->AddImage(ctx->dirty_texid, dirty_bb.Min + padding, dirty_bb.Max,
                                           ctx->uv0, ctx->uv1, GetColorU32(tint_col));
            }

            auto label_pos = bb.Min + padding;
            label_pos.y += ctx->size.y;
            RenderText(label_pos, ctx->str);

            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort | ImGuiHoveredFlags_NoSharedDelay))
            {
                if (ctx->tootip)
                {
                    // SetNextWindowSize({ 300, -FLT_MIN });
                    if (BeginTooltip())
                    {
                        ctx->tootip(ctx->user_data);
                        EndTooltip();
                    }
                }
            }

            return pressed;
        }

        static bool DragFileButton(FileButtonContext* ctx, const char* drag_type, string_view drag_data)
        {
            bool b = FileButton(ctx);
            if (drag_type)
            {
                if (ImGui::BeginDragDropSource())
                {
                    ImGui::Image(ctx->texture_id, ctx->size, ctx->uv0, ctx->uv1);
                    ImGui::SetDragDropPayload(drag_type, drag_data.data(), drag_data.size());
                    ImGui::EndDragDropSource();
                }
            }

            return b;
        }
    } // namespace PImGui

    void WorkspaceWindow::RenderFolderTree(SPtr<AssetFileNode> node)
    {
        ImGuiTreeNodeFlags base_flags =
            ImGuiTreeNodeFlags_OpenOnArrow |
            ImGuiTreeNodeFlags_OpenOnDoubleClick |
            ImGuiTreeNodeFlags_SpanFullWidth;

        if (!node->IsFolder)
        {
            return;
        }

        if (node->IsCollapsed)
        {
            base_flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        if (m_currentFolder == node->AssetPath)
        {
            base_flags |= ImGuiTreeNodeFlags_Selected;
        }

        bool isOpened = ImGui::TreeNodeEx(node->AssetName.c_str(), base_flags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            SetCurrentFolder(node->AssetPath);
        }

        if (isOpened)
        {
            if (node->IsFolder)
            {
                for (auto& i : node->GetChildren())
                {
                    RenderFolderTree(i);
                }
            }
        }

        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::GetDragDropPayload();
            string dragType;

            if(node->IsFolder && !std::strcmp(payload->DataType, "PULSARED_DRAG"))
            {
                const auto str = string_view(static_cast<char*>(payload->Data), payload->DataSize);

                auto strs = StringUtil::Split(str, ";");
                auto& intype = strs[0];
                auto& id = strs[1];

                if (payload = ImGui::AcceptDragDropPayload("PULSARED_DRAG"))
                {
                    if (intype == cltypeof<FolderAsset>()->GetName())
                    {
                    }
                    else
                    {
                        auto assetPath = AssetDatabase::GetPathById(ObjectHandle::parse(id));
                        auto assetName = AssetDatabase::AssetPathToAssetName(assetPath);
                        auto dstPath = StringUtil::Concat(node->AssetPath, "/", assetName);
                        AssetDatabase::Rename(assetPath, dstPath);
                    }
                }
            }

            ImGui::EndDragDropTarget();
        }

        if (isOpened)
        {
            ImGui::TreePop();
        }
    }

    void WorkspaceWindow::OnOpen()
    {
        base::OnOpen();
        m_onGetContextCallback = OnGetContext::FromWeakMember(self_weak(), &ThisClass::MakeMenuContext);
        MenuManager::RegisterContextProvider("Assets", m_onGetContextCallback);
    }
    void WorkspaceWindow::OnClose()
    {
        base::OnClose();
        MenuManager::UnregisterContextProvider("Assets", m_onGetContextCallback);
    }

    void WorkspaceWindow::OnDrawImGui(float dt)
    {
        if (!Workspace::IsOpened())
        {
            ImGui::Text("no project");
            return;
        }

        OnDrawBar();

        ImGui::Columns(2);

        if (!m_layoutColumnOffset.has_value())
        {
            m_layoutColumnOffset = ImGui::GetWindowWidth() * 0.3f;
        }
        else
        {
            m_layoutColumnOffset = ImGui::GetColumnOffset(1);
        }

        ImGui::SetColumnOffset(1, m_layoutColumnOffset.value());

        OnDrawFolderTree();

        ImGui::NextColumn();

        OnDrawContent();

        ImGui::Columns(1);
    }


    struct RenameDialog : ModalDialog
    {
        char m_inputBuffer[64]{};
        std::function<void(const char*)> m_okCallback;

        using ModalDialog::ModalDialog;

        void OnDraw(float dt) override
        {
            ImGui::Text("name:");
            ImGui::InputText("##x", m_inputBuffer, 64);

            if (ImGui::Button("ok"))
            {
                m_shouldClose = true;
                if (m_okCallback)
                    m_okCallback(m_inputBuffer);
            }
            ImGui::SameLine();
            if (ImGui::Button("cancel"))
            {
                m_shouldClose = true;
            }
        }

    };

    static void InitMainMenuAsset()
    {
        auto menu = mksptr(new MenuEntrySubMenu("Assets"));
        menu->Priority = 30;
        MenuManager::GetMainMenu()->AddEntry(menu);

        auto hasPathLambda = MenuCanOperate::FromLambda([](const MenuContexts_sp& ctxs) -> bool {
            if (AssetsMenuContext_sp ctx; ctxs && ((ctx = ctxs->FindContext<AssetsMenuContext>())))
            {
                if (!ctx->CurrentPath.empty())
                    return true;
            }
            return false;
        });

        // --------------- File ---------------
        {
            menu->AddEntry(mksptr(new MenuEntrySeparate("File")));
        }
        {
            auto entry = mksptr(new MenuEntrySubMenu("CreateAsset", "Create Asset"));
            entry->CanOperate = hasPathLambda;
            menu->AddEntry(entry);

            auto onAssetCreated = MenuAction::FromLambda([](MenuContexts_sp ctxs) {
                if (AssetsMenuContext_sp ctx; ctxs && (ctx = ctxs->FindContext<AssetsMenuContext>()))
                {
                    Type* type = AssemblyManager::GlobalFindType(ctxs->EntryName);
                    auto curPath = ctx->CurrentPath;

                    auto dialog = mksptr(new RenameDialog("New Asset"));
                    dialog->m_okCallback = [curPath, type](const char* str) {
                        AssetDatabase::NewAsset(curPath, str, type);
                    };
                    GetEdApp()->ShowModalDialog(dialog);
                }
            });

            for (Type* type : AssemblyManager::GlobalSearchType(cltypeof<AssetObject>()))
            {
                if (!type->IsDefinedAttribute(cltypeof<CreateAssetAttribute>()))
                {
                    continue;
                    ;
                }
                auto typePaths = StringUtil::Split(type->GetName(), "::");
                MenuEntrySubMenu_sp submenu = entry;
                for (size_t i = 0; i < typePaths.size(); i++)
                {
                    auto friendlyName = StringUtil::FriendlyName(typePaths[i]);
                    if (i == typePaths.size() - 1)
                    {
                        auto btn = mksptr(new MenuEntryButton(type->GetName(), friendlyName));
                        btn->Action = onAssetCreated;
                        btn->CanOperate = hasPathLambda;
                        submenu->AddEntry(btn);
                    }
                    else
                    {
                        auto name = typePaths[i];
                        MenuEntrySubMenu_sp newPathComponent = submenu->FindSubMenuEntry(name);
                        if (!newPathComponent)
                        {
                            newPathComponent = mksptr(new MenuEntrySubMenu(name, friendlyName));
                            submenu->AddEntry(newPathComponent);
                        }
                        submenu = newPathComponent;
                    }
                }
            }
        }

        {
            auto entry = mksptr(new MenuEntryButton("Create Folder"));
            entry->CanOperate = hasPathLambda;
            menu->AddEntry(entry);
        }

        {
            auto entry = mksptr(new MenuEntryButton("Import"));
            entry->CanOperate = hasPathLambda;
            menu->AddEntry(entry);
        }
        {
            auto entry = mksptr(new MenuEntryButton("Reimport"));
            entry->CanOperate = hasPathLambda;
            menu->AddEntry(entry);
        }
        // ------------- Common -------------
        {
            menu->AddEntry(mksptr(new MenuEntrySeparate("Common")));
        }
        {
            auto entry = mksptr(new MenuEntryButton("Rename"));
            entry->CanOperate = hasPathLambda;
            entry->Action = MenuAction::FromLambda([](MenuContexts_rsp) {

            });
            menu->AddEntry(entry);
        }
        {
            auto entry = mksptr(new MenuEntryButton("Duplicate"));
            entry->CanOperate = hasPathLambda;
            menu->AddEntry(entry);
        }
        {
            auto entry = mksptr(new MenuEntryButton("Save"));
            entry->CanOperate = hasPathLambda;
            entry->Action = MenuAction::FromLambda([](MenuContexts_rsp ctxs)
            {
                if (AssetsMenuContext_sp ctx; ctxs && ((ctx = ctxs->FindContext<AssetsMenuContext>())))
                {
                    for (const auto& selectedFile : ctx->SelectedFiles)
                    {
                        if (!selectedFile.lock()->IsFolder)
                        {
                            auto asset = AssetDatabase::LoadAssetAtPath(selectedFile.lock()->AssetPath);
                            AssetDatabase::Save(asset);
                        }
                    }
                }
            });
            menu->AddEntry(entry);
        }
        {
            auto entry = mksptr(new MenuEntryButton("Delete"));
            entry->CanOperate = hasPathLambda;
            entry->Action = MenuAction::FromLambda([](const MenuContexts_sp& ctxs) {
                if (AssetsMenuContext_sp ctx; ctxs && ((ctx = ctxs->FindContext<AssetsMenuContext>())))
                {
                    for (const auto& selectedFile : ctx->SelectedFiles)
                    {
                        if (selectedFile.lock()->IsFolder)
                        {
                            AssetDatabase::DeleteFolder(selectedFile.lock()->AssetPath);
                        }
                        else
                        {
                            AssetDatabase::DeleteAssets({selectedFile.lock()->AssetPath});
                        }
                    }
                }
            });
            menu->AddEntry(entry);
        }
        {
            auto entry = mksptr(new MenuEntryButton("Reload"));
            entry->CanOperate = hasPathLambda;
            entry->Action = MenuAction::FromLambda([](const MenuContexts_sp& ctxs) {
                if (AssetsMenuContext_sp ctx; ctxs && ((ctx = ctxs->FindContext<AssetsMenuContext>())))
                {
                    for (auto& item : ctx->SelectedFiles)
                    {
                        if (auto file = item.lock())
                        {
                            if (!file->IsFolder)
                            {
                                AssetDatabase::ReloadAsset(file->AssetMeta->Handle);
                            }
                        }
                    }
                }
            });
            menu->AddEntry(entry);
        }
        // ------------- explorer --------------
        {
            auto entry = mksptr(new MenuEntrySeparate("Explorer"));
            menu->AddEntry(entry);
        }
        {
            auto entry = mksptr(new MenuEntryButton("ShowExplorer", "Show In Explorer"));
            entry->CanOperate = hasPathLambda;
            entry->Action = MenuAction::FromLambda([](SPtr<MenuContexts>) {
                if (auto win = EditorWindowManager::GetPanelWindow(cltypeof<WorkspaceWindow>()))
                {
                    static_cast<WorkspaceWindow*>(win.get())->OpenExplorer();
                }
            });
            menu->AddEntry(entry);
        }
    }

    void WorkspaceWindow::ClearSelectedFile()
    {
        m_selectedFiles.clear();
    }
    void WorkspaceWindow::OnCurrentFolderChanged()
    {
        ClearSelectedFile();
    }
    void WorkspaceWindow::SetCurrentFolder(string_view path)
    {
        m_currentFolder = path;
        OnCurrentFolderChanged();
    }
    void WorkspaceWindow::OpenExplorer() const
    {
        jxcorlib::platform::system::OpenFileBrowser(GetCurrentPhysicsFolder());
    }
    std::filesystem::path WorkspaceWindow::GetCurrentPhysicsFolder() const
    {
        return AssetDatabase::AssetPathToPhysicsPath(m_currentFolder);
    }



    static bool _DropAssetTarget(const string& currentAssetPath)
    {
        bool result = false;
        if (ImGui::BeginDragDropTarget())
        {
            const ImGuiPayload* payload = ImGui::GetDragDropPayload();

            if (!std::strcmp(payload->DataType, "PULSARED_DRAG"))
            {
                const auto str = string_view(static_cast<char*>(payload->Data), payload->DataSize);
                auto strs = StringUtil::Split(str, ";");
                auto& inTypeName = strs[0];
                auto& id = strs[1];
                auto srcType = AssemblyManager::GlobalFindType(inTypeName);

                if (srcType->IsSubclassOf(cltypeof<AssetObject>()) || srcType == cltypeof<FolderAsset>())
                {
                    if (payload = ImGui::AcceptDragDropPayload("PULSARED_DRAG"))
                    {
                        if (srcType == cltypeof<FolderAsset>())
                        {
                            auto srcPath = id;
                            auto dstPath = currentAssetPath + "/" + AssetDatabase::AssetPathToAssetName(id);
                            AssetDatabase::Rename(id, dstPath);
                            result = true;
                        }
                        else
                        {
                            auto srcPath = AssetDatabase::GetPathById(ObjectHandle::parse(id));
                            auto dstPath = currentAssetPath + "/" + AssetDatabase::AssetPathToAssetName(srcPath);
                            AssetDatabase::Rename(srcPath, dstPath);
                            result = true;
                        }
                    }
                }
            }

            ImGui::EndDragDropTarget();

        }
        return result;
    }

    void WorkspaceWindow::OnDrawBar()
    {
        static bool a = false;
        if (a == false)
        {
            InitMainMenuAsset();
            a = true;
        }

        if (ImGui::BeginMenuBar())
        {

            if (m_currentFolder.empty())
            {
                ImGui::BeginDisabled();
            }

            if (ImGui::Button(ICON_FK_PLUS "  Add Asset"))
            {
                ImGui::OpenPopup("WorkspaceWindow.AddAsset");
            }
            if (ImGui::BeginPopup("WorkspaceWindow.AddAsset"))
            {
                ImGui::SeparatorText("Add Asset");

                auto menu = MenuManager::GetMainMenu()->FindSubMenuEntry("Assets");
                auto ctxs = mksptr(new MenuContexts());
                ctxs->Contexts.push_back(MakeMenuContext());
                MenuRenderer::RenderMenu(menu->FindSubMenuEntry("CreateAsset").get(), ctxs);

                ImGui::EndPopup();
            }

            if (ImGui::Button(ICON_FK_DOWNLOAD "  Import"))
            {
                this->OnClick_Import();
            }

            if (ImGui::Button(ICON_FK_FOLDER_OPEN "  Explorer"))
            {
                this->OpenExplorer();
            }
            if (m_currentFolder.empty())
            {
                ImGui::EndDisabled();
            }

            ImGui::Text("Path:  ");
            auto paths = StringUtil::Split(m_currentFolder, u8char("/"));
            ImGui::PushStyleColor(ImGuiCol_Button, {});
            string trackPathString;
            for (size_t i = 0; i < paths.size(); i++)
            {
                if (i != 0)
                {
                    trackPathString.push_back('/');
                }
                trackPathString.append(paths[i]);

                ImGui::PushID((int)i);
                if (ImGui::Button(paths[i].c_str()))
                {
                    SetCurrentFolder(trackPathString);
                }
                if (_DropAssetTarget(trackPathString))
                {

                }
                ImGui::Text("/");
                ImGui::PopID();
            }

            ImGui::PopStyleColor();

            ImGui::EndMenuBar();
        }
    }

    void WorkspaceWindow::OnDrawFolderTree()
    {
        if (ImGui::BeginChild("FolderTree"))
        {
            RenderFolderTree(AssetDatabase::FileTree);
        }
        ImGui::EndChild();
    }

    void WorkspaceWindow::OnDrawContent()
    {
        ImGui::InputTextWithHint("##Search", "Search", this->search_buf, sizeof(this->search_buf));
        ImGui::SameLine();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##IconSize", &m_iconSize, 30, 300);

        if (ImGui::BeginChild("FilesViewer"))
        {
            RenderFileContent();
        }
        ImGui::EndChild();
    }

    static void _RenderFileToolTips(void* data)
    {
        auto node = (AssetFileNode*)data;

        Type* assetType = node->GetAssetType();

        auto title = StringUtil::Concat(StringUtil::FriendlyName(node->AssetName), " ( ", StringUtil::FriendlyName(assetType->GetShortName()), " ) ");
        ImGui::SeparatorText(title.c_str());

        ImGui::Text(("Type: " + assetType->GetName()).c_str());

        ImGui::Text(("Path: " + node->AssetPath).c_str());
        ImGui::Text(("DiskPath: " + node->GetPhysicsPath()).c_str());
    }

    template <typename IT, typename V>
    static IT weaks_find(IT begin, IT end, V value)
    {
        for (auto it = begin; it != end; ++it)
        {
            if (it->lock() == value.lock())
            {
                return it;
            }
        }
        return end;
    }

    void WorkspaceWindow::RenderFileContent()
    {
        std::shared_ptr<AssetFileNode> p = AssetDatabase::FileTree->Find(m_currentFolder);

        static float padding = 16.f;
        const float cellSize = m_iconSize + padding;
        const float panelWidth = ImGui::GetContentRegionAvail().x;
        int columnCount = (int)(panelWidth / cellSize);
        if (columnCount < 1)
            columnCount = 1;

        ImGui::Columns(columnCount, 0, false);

        for (auto& child : p->GetChildren())
        {
            ImGui::PushStyleColor(ImGuiCol_Button, {});

            Type* assetType = child->GetAssetType();
            gfx::GFXDescriptorSet_wp descSet = AssetDatabase::IconPool->GetDescriptorSet({assetType->GetName()});
            gfx::GFXDescriptorSet_wp dirtySet = AssetDatabase::IconPool->GetDescriptorSet("WorkspaceWindow.Dirty");

            const bool isFolder = child->IsFolder;
            const auto isSelected = weaks_find(m_selectedFiles.begin(), m_selectedFiles.end(), std::weak_ptr{child}) != m_selectedFiles.end();
            const bool isDirty = isFolder ? false : AssetDatabase::IsDirty(child->AssetMeta->Handle);

            const auto iconSize = ImVec2(m_iconSize, m_iconSize);
            ImTextureID iconDesc = reinterpret_cast<void*>(descSet.lock()->GetId());
            ImTextureID dirtyDesc = reinterpret_cast<void*>(dirtySet.lock()->GetId());
            const auto dragType = "PULSARED_DRAG";
            const string dragData = StringUtil::Concat(
                child->GetAssetType()->GetName(),
                ";",
                isFolder ? child->AssetPath : child->AssetMeta->Handle.to_string());

            PImGui::FileButtonContext uictx{};
            uictx.str = child->AssetName.c_str();
            uictx.texture_id = iconDesc;
            uictx.size = iconSize;
            uictx.label_height = 30;
            uictx.selected = isSelected;
            uictx.is_dirty = isDirty;
            uictx.dirty_texid = dirtyDesc;
            uictx.user_data = child.get();
            uictx.tootip = &_RenderFileToolTips;
            uictx.is_folder = isFolder;

            if (PImGui::DragFileButton(&uictx, dragType, dragData))
            {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    if (child->IsFolder)
                    {
                        SetCurrentFolder(child->AssetPath);
                    }
                    else
                    {
                        Logger::Log("open asset editor: " + assetType->GetName(), LogLevel::Info);
                        AssetUtil::OpenAssetEditor(AssetDatabase::LoadAssetAtPath(child->AssetPath));
                    }
                }
                else if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    if (ImGui::GetIO().KeyShift)
                    {
                        auto it = weaks_find(m_selectedFiles.begin(), m_selectedFiles.end(), std::weak_ptr{child});
                        if (it != m_selectedFiles.end())
                        {
                            m_selectedFiles.erase(it);
                        }
                        else
                        {
                            m_selectedFiles.push_back(child);
                        }
                    }
                    else
                    {
                        m_selectedFiles.clear();
                        m_selectedFiles.push_back(child);
                    }
                }
            }
            if (isFolder)
            {
                if (_DropAssetTarget(child->AssetPath))
                {

                }
            }
            // if (isFolder && ImGui::BeginDragDropTarget())
            // {
            //     const ImGuiPayload* payload = ImGui::GetDragDropPayload();
            //
            //     if (!std::strcmp(payload->DataType, "PULSARED_DRAG"))
            //     {
            //         const auto str = string_view(static_cast<char*>(payload->Data), payload->DataSize);
            //
            //         auto strs = StringUtil::Split(str, ";");
            //         auto& inTypeName = strs[0];
            //         auto& id = strs[1];
            //         auto srcType = AssemblyManager::GlobalFindType(inTypeName);
            //
            //         if (srcType->IsSubclassOf(cltypeof<AssetObject>()) || srcType == cltypeof<FolderAsset>())
            //         {
            //             if (payload = ImGui::AcceptDragDropPayload("PULSARED_DRAG"))
            //             {
            //                 if (srcType == cltypeof<FolderAsset>())
            //                 {
            //                     auto srcPath = id;
            //                     auto dstPath = child->AssetPath + "/" + AssetDatabase::AssetPathToAssetName(id);
            //                     AssetDatabase::Rename(id, dstPath);
            //                 }
            //                 else
            //                 {
            //                     auto srcPath = AssetDatabase::GetPathById(ObjectHandle::parse(id));
            //                     auto dstPath = child->AssetPath + "/" + AssetDatabase::AssetPathToAssetName(srcPath);
            //                     AssetDatabase::Rename(srcPath, dstPath);
            //                 }
            //             }
            //         }
            //     }
            //
            //     ImGui::EndDragDropTarget();
            // }

            ImGui::PopStyleColor();

            // if (isDrawing)
            ImGui::NextColumn();
        }
        ImGui::Columns(1);

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::OpenPopup("WorkspaceWindow.AssetContext");
        }
        if (ImGui::BeginPopup("WorkspaceWindow.AssetContext"))
        {
            const auto menu = MenuManager::GetMainMenu()->FindSubMenuEntry("Assets");
            const auto ctxs = mksptr(new MenuContexts());
            ctxs->Contexts.push_back(MakeMenuContext());
            MenuRenderer::RenderMenu(menu.get(), ctxs);

            ImGui::EndPopup();
        }
    }
    MenuContextBase_sp WorkspaceWindow::MakeMenuContext()
    {
        auto ctx = mksptr(new AssetsMenuContext{m_currentFolder});
        ctx->CurrentPath = m_currentFolder;
        ctx->SelectedFiles = m_selectedFiles;
        return ctx;
    }
    void WorkspaceWindow::OnClick_Import()
    {
        const auto mainWindow = jxcorlib::platform::window::GetMainWindowHandle();
        fs::path selectedFileName;

        string filterStr;
        for (const auto factory : AssetImporterFactoryManager::GetFactories())
        {
            filterStr += StringUtil::Concat(factory->GetDescription(), "(", factory->GetFilter(), ")", ";", factory->GetFilter(), ";");
        }

        if (jxcorlib::platform::window::OpenFileDialog(mainWindow, filterStr, "", &selectedFileName))
        {
            const auto factory = AssetImporterFactoryManager::FindFactoryByExt(selectedFileName.extension().string());
            if (!factory)
            {
                Logger::Log("no import factory.", LogLevel::Error);
                return;
            }
            const auto settings = factory->CreateImporterSettings();
            settings->TargetPath = m_currentFolder; // current target
            settings->ImportFiles->push_back(StringUtil::StringCast(selectedFileName.generic_u8string()));

            factory->CreateImporter()->Import(settings.get());
        }
    }

    WorkspaceWindow::WorkspaceWindow()
    {
        ::memset(this->search_buf, 0, sizeof(this->search_buf));
    }
} // namespace pulsared